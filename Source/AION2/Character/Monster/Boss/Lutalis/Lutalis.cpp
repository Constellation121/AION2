// Fill out your copyright notice in the Description page of Project Settings.

#include "Character/Monster/Boss/Lutalis/Lutalis.h"

#include "AIController.h"
#include "AbilitySystemComponent.h"
#include "Character/Monster/Boss/Lutalis/LutalisElectricZone.h"
#include "EngineUtils.h"
#include "Engine/World.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameplayTagContainer.h"
#include "GAS/AttributeSet/AOAttributeSet.h"

#include "AI/AIMonsterControllerBase.h"
#include "GAS/AOGameplayTags.h"

ALutalis::ALutalis(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	DungeonBossIndex = 2;
}

void ALutalis::BeginPlay()
{
	Super::BeginPlay();

	InitAttributeSet();
}

void ALutalis::InitAttributeSet()
{
	Super::InitAttributeSet();

	AttributeSet->InitHealth(5000.f);
	AttributeSet->InitMaxHealth(5000.f);

	AttributeSet->InitStamina(100.f);
	AttributeSet->InitMaxStamina(100.f);
}

void ALutalis::EndGroggy()
{
	if (!HasAuthority() || !bIsGroggy || bIsDead)
	{
		return;
	}

	bIsGroggy = false;

	if (AttributeSet)
	{
		AttributeSet->SetGroggy(AttributeSet->GetMaxGroggy());
	}



	AAIMonsterControllerBase* pMonsterController = Cast<AAIMonsterControllerBase>(GetController());
	if (pMonsterController == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("pMonsterController nullptr"));
	}

	// 만약 PreCombat 페이즈를 안쓰신다면 EndGroggy를 virtual 함수로 선언하신 뒤 
	// Set_Phase를 다른걸로 사용하시면 될 거 같습니다.
	pMonsterController->Set_Phase(PHASE_MONSTER_COMBAT);
}

bool ALutalis::BeginElectricRangeWarning(float WarningDuration)
{
	if (!HasAuthority())
	{
		return false;
	}

	if (PreparedElectricZoneArcIndex != INDEX_NONE)
	{
		return BeginPreparedElectricRangeWarning(WarningDuration);
	}

	if (!ResolveElectricZoneActor())
	{
		return false;
	}

	ElectricZoneActor->InitZone(
		this,
		ElectricZoneTickDamageData,
		ElectricZoneRadius,
		ElectricZoneSectorAngle
	);

	const int32 SelectedArcIndex = ElectricZoneActor->StartRandomArcWarning(WarningDuration);
	return SelectedArcIndex != INDEX_NONE;
}

bool ALutalis::BeginPreparedElectricRangeWarning(float WarningDuration)
{
	if (!HasAuthority())
	{
		return false;
	}

	if (!ElectricZoneActor)
	{
		return false;
	}

	if (PreparedElectricZoneArcIndex == INDEX_NONE)
	{
		return false;
	}

	const bool bStarted =
		ElectricZoneActor->StartArcWarning(PreparedElectricZoneArcIndex, WarningDuration);

	if (bStarted)
	{
		PreparedElectricZoneArcIndex = INDEX_NONE;
	}

	return bStarted;
}

bool ALutalis::ActivateElectricRangeDamage()
{
	if (!HasAuthority())
	{
		return false;
	}

	if (!ElectricZoneActor)
	{
		return false;
	}

	return ElectricZoneActor->ActivatePendingArc();
}

// BP에서 작동 시킴.
void ALutalis::ConsumeElectricZoneRequestTag()
{
	if (!HasAuthority())
		return;

	UAbilitySystemComponent* ASC = GetAbilitySystemComponent();
	if (!ASC)
		return;

	const FGameplayTag ElectricZoneRequestTag =
		FGameplayTag::RequestGameplayTag(TEXT("State.Monster.LT.ElectricZone"));

	ASC->SetLooseGameplayTagCount(ElectricZoneRequestTag, 0);
}

bool ALutalis::PrepareElectricZoneCast()
{
	if (!HasAuthority())
	{
		return false;
	}

	if (!ResolveElectricZoneActor())
	{
		return false;
	}

	ElectricZoneActor->InitZone(
		this,
		ElectricZoneTickDamageData,
		ElectricZoneRadius,
		ElectricZoneSectorAngle
	);

	PreparedElectricZoneArcIndex = ElectricZoneActor->SelectRandomUnusedArcIndex();

	if (PreparedElectricZoneArcIndex == INDEX_NONE)
	{
		UE_LOG(LogTemp, Warning, TEXT("[Lutalis] Failed to select ElectricZone arc index."));
		return false;
	}

	float TargetYaw = 0.0f;
	if (!ElectricZoneActor->GetArcIndicatorForwardYaw(PreparedElectricZoneArcIndex, TargetYaw))
	{
		PreparedElectricZoneArcIndex = INDEX_NONE;
		return false;
	}

	PreparedElectricZoneTargetYaw = FRotator::NormalizeAxis(TargetYaw + ElectricZoneFacingYawOffset);

	if (AAIController* AIController = Cast<AAIController>(GetController()))
	{
		AIController->StopMovement();
		AIController->ClearFocus(EAIFocusPriority::Gameplay);
	}

	if (UCharacterMovementComponent* MovementComponent = GetCharacterMovement())
	{
		MovementComponent->StopMovementImmediately();
	}

	return true;
}

bool ALutalis::TickRotateToPreparedElectricZoneYaw(float DeltaTime)
{
	if (PreparedElectricZoneArcIndex == INDEX_NONE)
	{
		return false;
	}

	const FRotator CurrentRotation = GetActorRotation();

	FRotator TargetRotation = CurrentRotation;
	TargetRotation.Yaw = PreparedElectricZoneTargetYaw;

	const FRotator NewRotation = FMath::RInterpConstantTo(
		CurrentRotation,
		TargetRotation,
		DeltaTime,
		ElectricZoneRotateSpeed
	);

	SetActorRotation(NewRotation);

	if (AController* CurrentController = GetController())
	{
		CurrentController->SetControlRotation(NewRotation);
	}

	const float DeltaYaw = FMath::Abs(
		FMath::FindDeltaAngleDegrees(NewRotation.Yaw, PreparedElectricZoneTargetYaw)
	);

	if (DeltaYaw > ElectricZoneRotateTolerance)
	{
		return false;
	}

	SetActorRotation(TargetRotation);

	if (AController* CurrentController = GetController())
	{
		CurrentController->SetControlRotation(TargetRotation);
	}

	return true;
}

void ALutalis::ResetPreparedElectricZoneCast()
{
	PreparedElectricZoneArcIndex = INDEX_NONE;
	PreparedElectricZoneTargetYaw = 0.0f;
}

bool ALutalis::ResolveElectricZoneActor()
{
	if (ElectricZoneActor)
	{
		return true;
	}

	if (UWorld* World = GetWorld())
	{
		for (TActorIterator<ALutalisElectricZone> It(World); It; ++It)
		{
			ElectricZoneActor = *It;
			break;
		}
	}

	if (!ElectricZoneActor)
	{
		UE_LOG(LogTemp, Warning, TEXT("[Lutalis] ElectricZoneActor is null."));
		return false;
	}

	return true;
}
