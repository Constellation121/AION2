// Fill out your copyright notice in the Description page of Project Settings.

#include "Character/Monster/AOMonsterBase.h"

#include "Game/AODunGameMode.h"

#include "AIController.h"
#include "BrainComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "NavigationSystem.h"

// Sets default values
AAOMonsterBase::AAOMonsterBase(const FObjectInitializer& ObjectInitializer)
    :Super(ObjectInitializer)
{
    // Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
}

// Called when the game starts or when spawned
void AAOMonsterBase::BeginPlay()
{
    Super::BeginPlay();

}


// Called every frame
void AAOMonsterBase::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AAOMonsterBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

}

// Boss Health 0 -> Call
// Only Server Call
void AAOMonsterBase::HandleBossDeath()
{
	UE_LOG(
		LogTemp,
		Error,
		TEXT("[BossDeath] HandleBossDeath Called! Boss: %s"),
		*GetName()
	);


	if (!HasAuthority())
	{
		return;
	}

	AAODunGameMode* DungeonGameMode =
		GetWorld()->GetAuthGameMode<AAODunGameMode>();

	if (!DungeonGameMode)
	{
		UE_LOG(
			LogTemp,
			Error,
			TEXT("[Dungeon] DungeonGameMode not found. Boss: %s"),
			*GetName()
		);
		return;
	}

	DungeonGameMode->NotifyBossDefeated(this);
}

void AAOMonsterBase::SetDungeonBossActive(bool bActive)
{
	// 외형 표시 여부
	SetActorHiddenInGame(!bActive);

	// Capsule Collision도 함께 제어
	if (UCapsuleComponent* Capsule = GetCapsuleComponent())
	{
		Capsule->SetCollisionEnabled(
			bActive
			? ECollisionEnabled::QueryAndPhysics
			: ECollisionEnabled::NoCollision
		);
	}

	// Tick을 꺼야 AI 관련 Tick이나 행동이 남지 않는다.
	SetActorTickEnabled(bActive);

	AAIController* AIController = Cast<AAIController>(GetController());

	if (!AIController)
	{
		UE_LOG(
			LogTemp,
			Warning,
			TEXT("[Dungeon] AIController is null: %s"),
			*GetName()
		);
		return;
	}

	if (!AIController->BrainComponent)
	{
		UE_LOG(
			LogTemp,
			Warning,
			TEXT("[Dungeon] BrainComponent is null: %s"),
			*GetName()
		);
		return;
	}

	if (!bActive)
	{
		AIController->StopMovement();

		AIController->BrainComponent->StopLogic(
			TEXT("Dungeon Boss Disabled")
		);

		return;
	}

	AIController->BrainComponent->RestartLogic();

	UE_LOG(
		LogTemp,
		Warning,
		TEXT("[Dungeon] Boss AI Activated: %s"),
		*GetName()
	);
}


bool AAOMonsterBase::CanMoveOnNavMesh(const FVector Direction, float Distance)
{
    const UNavigationSystemV1* NavSys = UNavigationSystemV1::GetCurrent(GetWorld());

    if (!NavSys)
        return false;

    FVector Dir2D = Direction;
    Dir2D.Z = 0.f;

    if (!Dir2D.Normalize())
        return false;

    // Start를 발바닥 높이로 수정  
    const float HalfHeight = GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
    FVector Start = GetActorLocation();
    Start.Z -= HalfHeight;

    const FVector End = Start + Dir2D * Distance;

    FVector HitLocation;
    const bool bHit = NavSys->NavigationRaycast(GetWorld(), Start, End, HitLocation);


    return !bHit;
}