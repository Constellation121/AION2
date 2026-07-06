// Fill out your copyright notice in the Description page of Project Settings.


#include "Utils/AODungeonEntrance.h"
#include "Blueprint/UserWidget.h"
#include "Character/Daeva/Daeva.h"
#include "Components/BoxComponent.h"
#include "Player/AOPlayerController.h"
#include "Manager/AOUIManager.h"
#include "Character/ServerCharacter/MMODaeva.h"
#include "UI/AODungeonEntranceWidget.h"


// Sets default values
AAODungeonEntrance::AAODungeonEntrance()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBoxComponent"));

	if (TriggerBox)
	{
		TriggerBox->SetBoxExtent(FVector(40.f, 30.f, 50.f));
		TriggerBox->SetCollisionProfileName(TEXT("Trigger"));
	}

	EffectComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("EffectComponent"));
	EffectComponent->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void AAODungeonEntrance::BeginPlay()
{
	Super::BeginPlay();

	TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &AAODungeonEntrance::OnOverlapBegin);
	TriggerBox->OnComponentEndOverlap.AddDynamic(this, &AAODungeonEntrance::OnOverlapEnd);

	RootComponent = TriggerBox;

	if (DungeonEffect && EffectComponent)
	{
		EffectComponent->SetAsset(DungeonEffect);
		EffectComponent->Activate();
	}
	UIManager = GetGameInstance()->GetSubsystem<UAOUIManager>();
}

// Called every frame
void AAODungeonEntrance::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AAODungeonEntrance::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
	const FHitResult& SweepResult)
{
	ADaeva* CharacterActor = Cast<ADaeva>(OtherActor);
	if (CharacterActor != nullptr)
	{
		UE_LOG(LogTemp, Log, TEXT("Dungeon OnOverlapBegin"));
		PC = Cast<AAOPlayerController>(CharacterActor->GetController());
		if (PC)
		{
			DungeonWaitingRoomWidget = UIManager->ShowWidget(DungeonWaitingRoomClass, EUILayer::PopUp);
			EnableInput(PC);

			if (InputComponent && DungeonWaitingRoomWidget)
			{
				OverlappedPlayer = OtherActor;
				InputComponent->BindKey(EKeys::F, IE_Pressed, this, &AAODungeonEntrance::EnterDungeonWaitingRoom);
			}
		}
	}
}

void AAODungeonEntrance::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	ADaeva* CharacterActor = Cast<ADaeva>(OtherActor);
	if (CharacterActor != nullptr)
		PC = Cast<AAOPlayerController>(CharacterActor->GetController());

	if (PC)
	{
		DisableInput(PC);
		PC->SetShowMouseCursor(false);
		UIManager->HideWidget(DungeonWaitingRoomWidget);
	}
}

void AAODungeonEntrance::EnterDungeon()
{
}

void AAODungeonEntrance::EnterDungeonWaitingRoom()
{
	// Exception Handler: 객체가 아직 없거나 유효하지 않은 상태면 Crash 막기
	if (!UIManager || !PC)
	{
		return;
	}
	if(DungeonWaitingRoomWidget)
		UIManager->HideWidget(DungeonWaitingRoomWidget);

	DungeonRoomWidget = UIManager->ShowWidget(DungeonRoomClass, EUILayer::System);

	UAODungeonEntranceWidget* DungeonEntranceWidget =
		Cast<UAODungeonEntranceWidget>(DungeonRoomWidget);

	if (DungeonEntranceWidget)
	{
		DungeonEntranceWidget->InitializeWaitingRoom();
	}

	if (PC)
	{
		PC->SetShowMouseCursor(true);
		AMMODaeva* Owner = Cast<AMMODaeva>(PC->GetPawn());
		if (Owner)
		{
			Owner->SendDungeonWait();
		}
	}
}
