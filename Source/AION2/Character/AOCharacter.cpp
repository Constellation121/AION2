#include "Character/AOCharacter.h"
#include "Character/AOCharacterMovementComponent.h"
#include "Physics/Collision.h"
#include "GAS/AOGameplayTags.h"
#include "Player/AOPlayerController.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"

AAOCharacter::AAOCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UAOCharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
{
	PrimaryActorTick.bCanEverTick = true;
}

UAbilitySystemComponent* AAOCharacter::GetAbilitySystemComponent() const
{
	return ASC;
}

void AAOCharacter::Multicast_DrawDebugCapsuleCollider_Implementation(const FVector& CapsuleOrigin, const float CapsuleHalfHeight, const float AttackRadius, const FColor DrawColor)
{
	DrawDebugCapsuleCollider(CapsuleOrigin, CapsuleHalfHeight, AttackRadius, DrawColor);
}

void AAOCharacter::Client_PlayCameraShake_Implementation()
{
	APlayerController* PC = Cast<APlayerController>(GetController());
	if (!PC || !CameraShakeClass)
	{
		return;
	}

	PC->ClientStartCameraShake(CameraShakeClass);
}

void AAOCharacter::CheckAttackHit(const FAttackData& AttackData)
{
	TArray<FHitResult> OutHitResults;

	const float AttackRange = AttackData.TraceData.Range;
	const float AttackRadius = AttackData.TraceData.Radius;

	FCollisionQueryParams Params(SCENE_QUERY_STAT(Attack), false, this);

	FVector OriginalStart = GetActorTransform().TransformPosition(AttackData.TraceData.StartOffset);
	FVector OriginalEnd = OriginalStart + AttackData.TraceData.Direction.GetSafeNormal() * AttackRange;
	FVector CapsuleOrigin = OriginalStart + (OriginalEnd - OriginalStart) * 0.5f;

	bool bHitDetected = GetWorld()->SweepMultiByChannel(OutHitResults, OriginalStart, OriginalEnd, FQuat::Identity, CCHANNEL_ATTACK, FCollisionShape::MakeSphere(AttackRadius), Params);

	if (CVarDrawAttackTrace.GetValueOnGameThread())
	{
		const float CapsuleHalfHeight = AttackRange * 0.5f;
		FColor DrawColor = bHitDetected ? FColor::Green : FColor::Red;
		Multicast_DrawDebugCapsuleCollider(CapsuleOrigin, CapsuleHalfHeight, AttackRadius, DrawColor);
		UE_LOG(LogTemp, Log, TEXT("%f"), AttackRadius);
	}

	if (!bHitDetected)
	{
		return;
	}

	bool bDidShakeCamera = false;

	for (const FHitResult& HitResult : OutHitResults)
	{
		AActor* HitActor = HitResult.GetActor();
		if (!IsValid(HitActor))
		{
			continue;
		}
		
		if (!IsEnemy(HitActor))
		{
			continue;
		}

		OnAttackSucceeded(AttackData, HitActor, HitResult, bDidShakeCamera);
	}
}

void AAOCharacter::OnAttackSucceeded(const FAttackData& AttackData, AActor* HitActor, const FHitResult& HitResult, bool& bDidShakeCamera)
{
	AAOCharacter* Target = Cast<AAOCharacter>(HitActor);
	if (!Target)
	{
		return;
	}

	//Target->TakeDamageAO(AttackData, this);
	// gc
	PlayCameraShake(bDidShakeCamera);
}

void AAOCharacter::InitGAS()
{
}

void AAOCharacter::ClearGAS()
{
}

void AAOCharacter::TakeDamageAO(const FAttackData& AttackData, AAOCharacter* DamageCauser)
{
	UAbilitySystemComponent* SourceASC = DamageCauser->GetAbilitySystemComponent();
	UAbilitySystemComponent* TargetASC = GetAbilitySystemComponent();
	if (!SourceASC || !TargetASC)
	{
		return;
	}

	FGameplayEffectContextHandle Context = SourceASC->MakeEffectContext();
	FGameplayEffectSpecHandle SpecHandle = SourceASC->MakeOutgoingSpec(DamageEffect, 1, Context);
	SourceASC->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data.Get(), TargetASC);
}

bool AAOCharacter::IsEnemy(AActor* TargetActor)
{
	AAOCharacter* AOCharacter = Cast<AAOCharacter>(TargetActor);
	if (!AOCharacter)
	{
		return false;
	}

	UAbilitySystemComponent* MyASC = GetAbilitySystemComponent();
	UAbilitySystemComponent* TargetASC = AOCharacter->GetAbilitySystemComponent();
	if (!MyASC || !TargetASC)
	{
		return false;
	}

	if (MyASC->HasMatchingGameplayTag(TEAM_DAEVA))
	{
		return TargetASC->HasMatchingGameplayTag(TEAM_MONSTER);
	}

	if (MyASC->HasMatchingGameplayTag(TEAM_MONSTER))
	{
		return TargetASC->HasMatchingGameplayTag(TEAM_DAEVA);
	}

	return false;
}

void AAOCharacter::PlayCameraShake(bool& bDidShakeCamera)
{
	if (!bDidShakeCamera)
	{
		Client_PlayCameraShake();

		bDidShakeCamera = true;
	}
}

void AAOCharacter::DrawDebugCapsuleCollider(const FVector& CapsuleOrigin, const float CapsuleHalfHeight, const float AttackRadius, const FColor DrawColor)
{
#if ENABLE_DRAW_DEBUG
	DrawDebugCapsule(GetWorld(), CapsuleOrigin, CapsuleHalfHeight, AttackRadius, FRotationMatrix::MakeFromZ(GetActorForwardVector()).ToQuat(), DrawColor, false, 1.0f);
#endif
}
