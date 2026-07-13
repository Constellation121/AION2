#include "Character/Daeva/AOTrap.h"
#include "AOTrap.h"
#include "Character/AOCharacter.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "GameplayEffect.h"
#include "TimerManager.h"

AAOTrap::AAOTrap()
{
	bReplicates = true;
}

void AAOTrap::BeginPlay()
{
	// 장판 생성 후 부모의 직격 처리 / Destroy를 실행하기 위해 먼저 등록
	if (HasAuthority())
	{
		Collision->OnComponentBeginOverlap.AddDynamic(this, &AAOTrap::SpawnTrapOnOverlap);
	}

	Super::BeginPlay();
}

void AAOTrap::SpawnTrapOnOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!HasAuthority() || bTrapSpawned || !TrapZoneClass)
	{
		return;
	}

	AAOCharacter* HitCharacter = Cast<AAOCharacter>(OtherActor);

	bTrapSpawned = true;

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = DamageCauser.Get();
	SpawnParams.Instigator = DamageCauser.Get();
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	GetWorld()->SpawnActor<AAOTrapZone>(TrapZoneClass, GetActorLocation(), FRotator::ZeroRotator, SpawnParams);
	UE_LOG(LogTemp, Warning, TEXT("[Trap] Zone Spawned At : %s"), *GetActorLocation().ToString());
}