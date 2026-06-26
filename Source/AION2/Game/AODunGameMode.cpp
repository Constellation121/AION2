#include "Game/AODunGameMode.h"

#include "Character/Monster/AOMonsterBase.h"
#include "Game/AOGameInstance.h"
#include "Manager/AONetworkManager.h"

#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"

AAODunGameMode::AAODunGameMode()
{
	bUseSeamlessTravel = true;
	PrimaryActorTick.bCanEverTick = true;
}

void AAODunGameMode::BeginPlay()
{
	Super::BeginPlay();

	GameInst = Cast<UAOGameInstance>(GetWorld()->GetGameInstance());

	if (!GameInst)
	{
		UE_LOG(LogTemp, Error, TEXT("[Dungeon] GameInstance is invalid."));
		return;
	}

	NetworkManager = GameInst->GetNetworkManager();

	FindPlacedBosses();
	InitializePlacedBosses();

	StartDungeon();
}

void AAODunGameMode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (NetworkManager)
	{
		NetworkManager->ProcessQueuePackets();
	}
}

void AAODunGameMode::FindPlacedBosses()
{
	TArray<AActor*> FoundActors;

	UGameplayStatics::GetAllActorsOfClass(
		this,
		AAOMonsterBase::StaticClass(),
		FoundActors
	);

	for (AActor* Actor : FoundActors)
	{
		AAOMonsterBase* Boss = Cast<AAOMonsterBase>(Actor);

		if (!Boss)
		{
			continue;
		}

		const int32 BossIndex = Boss->DungeonBossIndex;

		// 0은 일반 몬스터, 1~3만 던전 보스
		if (BossIndex < 1 || BossIndex > 3)
		{
			continue;
		}

		if (PlacedBosses.Contains(BossIndex))
		{
			UE_LOG(
				LogTemp,
				Error,
				TEXT("[Dungeon] Boss %d is duplicated: %s"),
				BossIndex,
				*Boss->GetName()
			);
			continue;
		}

		PlacedBosses.Add(BossIndex, Boss);

		UE_LOG(
			LogTemp,
			Warning,
			TEXT("[Dungeon] Found Boss %d: %s"),
			BossIndex,
			*Boss->GetName()
		);
	}

	for (int32 BossIndex = 1; BossIndex <= 3; ++BossIndex)
	{
		if (!PlacedBosses.Contains(BossIndex))
		{
			UE_LOG(
				LogTemp,
				Error,
				TEXT("[Dungeon] Boss %d is not placed in this map."),
				BossIndex
			);
		}
	}
}

void AAODunGameMode::InitializePlacedBosses()
{
	for (const TPair<int32, TObjectPtr<AAOMonsterBase>>& Pair : PlacedBosses)
	{
		AAOMonsterBase* Boss = Pair.Value;

		if (!Boss)
		{
			continue;
		}

		// Boss1도 StartDungeon에서 켜기 때문에 시작 시 전부 꺼 둔다.
		Boss->SetDungeonBossActive(false);
	}
}

void AAODunGameMode::StartDungeon()
{
	if (CurrentPhase != EDungeonPhase::Ready)
	{
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("[Dungeon] Start Dungeon"));

	StartBossPhase(1);
}

void AAODunGameMode::StartBossPhase(int32 BossNumber)
{
	if (BossNumber < 1 || BossNumber > 3)
	{
		UE_LOG(
			LogTemp,
			Error,
			TEXT("[Dungeon] Invalid BossNumber: %d"),
			BossNumber
		);
		return;
	}

	CurrentBossNumber = BossNumber;

	SetCombatPhase(BossNumber);

	ActivateBoss(BossNumber);

	UE_LOG(
		LogTemp,
		Warning,
		TEXT("[Dungeon] Boss %d Combat Start"),
		BossNumber
	);
}

void AAODunGameMode::ActivateBoss(int32 BossNumber)
{
	AAOMonsterBase* Boss = PlacedBosses.FindRef(BossNumber);

	if (!Boss)
	{
		UE_LOG(
			LogTemp,
			Error,
			TEXT("[Dungeon] Boss %d not found."),
			BossNumber
		);
		return;
	}

	CurrentBoss = Boss;

	Boss->SetDungeonBossActive(true);

	UE_LOG(
		LogTemp,
		Warning,
		TEXT("[Dungeon] Boss %d Activated: %s"),
		BossNumber,
		*Boss->GetName()
	);
}

void AAODunGameMode::NotifyBossDefeated(AAOMonsterBase* DefeatedBoss)
{
	if (!DefeatedBoss)
	{
		return;
	}

	if (CurrentPhase == EDungeonPhase::Cleared ||
		CurrentPhase == EDungeonPhase::Failed)
	{
		return;
	}

	if (DefeatedBoss != CurrentBoss)
	{
		UE_LOG(
			LogTemp,
			Warning,
			TEXT("[Dungeon] Defeated boss is not current boss: %s"),
			*DefeatedBoss->GetName()
		);
		return;
	}

	UE_LOG(
		LogTemp,
		Warning,
		TEXT("[Dungeon] Boss %d Defeated"),
		CurrentBossNumber
	);

	SetDefeatedPhase(CurrentBossNumber);

	// 사망 애니메이션을 보여줄 거면 여기서 바로 끄지 말고,
	// 몽타주 종료 시점에 SetDungeonBossActive(false)를 호출하면 된다.
	DefeatedBoss->SetDungeonBossActive(false);

	CurrentBoss = nullptr;

	if (CurrentBossNumber < 3)
	{
		OpenGateForNextBoss(CurrentBossNumber);

		GetWorldTimerManager().SetTimer(
			NextBossTimerHandle,
			this,
			&AAODunGameMode::StartNextBoss,
			3.0f,
			false
		);

		return;
	}

	ClearDungeon();
}

void AAODunGameMode::StartNextBoss()
{
	StartBossPhase(CurrentBossNumber + 1);
}

void AAODunGameMode::ClearDungeon()
{
	CurrentPhase = EDungeonPhase::Cleared;

	UE_LOG(LogTemp, Warning, TEXT("[Dungeon] Dungeon Clear"));

	GiveDungeonReward();
	ActivateReturnPortal();
}

void AAODunGameMode::ReturnToVillage()
{
	if (VillageMapPath.IsEmpty())
	{
		UE_LOG(LogTemp, Error, TEXT("[Dungeon] VillageMapPath is Empty"));
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("[Dungeon] Return To Village"));

	GetWorld()->ServerTravel(VillageMapPath);
}

void AAODunGameMode::SetCombatPhase(int32 BossNumber)
{
	switch (BossNumber)
	{
	case 1:
		CurrentPhase = EDungeonPhase::Boss1Combat;
		break;

	case 2:
		CurrentPhase = EDungeonPhase::Boss2Combat;
		break;

	case 3:
		CurrentPhase = EDungeonPhase::Boss3Combat;
		break;

	default:
		break;
	}
}

void AAODunGameMode::SetDefeatedPhase(int32 BossNumber)
{
	switch (BossNumber)
	{
	case 1:
		CurrentPhase = EDungeonPhase::Boss1Defeated;
		break;

	case 2:
		CurrentPhase = EDungeonPhase::Boss2Defeated;
		break;

	case 3:
		CurrentPhase = EDungeonPhase::Boss3Defeated;
		break;

	default:
		break;
	}
}