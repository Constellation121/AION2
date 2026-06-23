#include "Character/AOCharacter.h"
#include "Character/AOCharacterMovementComponent.h"
#include "Physics/Collision.h"
#include "GAS/AOGameplayTags.h"
#include "Player/AOPlayerController.h"

#include "AbilitySystemComponent.h"

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

void AAOCharacter::CheckAttackHit(const FAttackData& AttackData)
{
	TArray<FHitResult> OutHitResults;

	const float AttackRange = AttackData.TraceData.Range;
	const float AttackRadius = AttackData.TraceData.Radius;

	FCollisionQueryParams Params(SCENE_QUERY_STAT(Attack), false, this);

	FVector OriginalStart = GetActorTransform().TransformPosition(AttackData.TraceData.StartOffset);
	FVector OriginalEnd = OriginalStart + AttackData.TraceData.Direction * AttackRange;
	FVector CapsuleOrigin = OriginalStart + (OriginalEnd - OriginalStart) * 0.5f;

	//float SweepHalfLength = FMath::Max(0.0f, (AttackRange * 0.5f) - AttackRadius);
	//FVector SweepStart = CapsuleOrigin - (GetActorForwardVector() * SweepHalfLength);
	//FVector SweepEnd = CapsuleOrigin + (GetActorForwardVector() * SweepHalfLength);

	bool bHitDetected = GetWorld()->SweepMultiByChannel(OutHitResults, OriginalStart, OriginalEnd, FQuat::Identity, CCHANNEL_ATTACK, FCollisionShape::MakeSphere(AttackRadius), Params);

	if (!bHitDetected)
	{
		return;
	}

	if (Cast<AAOPlayerController>(GetWorld()->GetFirstPlayerController())->GetShowColliderDebug())
	{
		const float CapsuleHalfHeight = AttackRange * 0.5f;
		FColor DrawColor = bHitDetected ? FColor::Green : FColor::Red;
		Multicast_DrawDebugCapsuleCollider(CapsuleOrigin, CapsuleHalfHeight, AttackRadius, DrawColor);
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
	// ge
	// gc
	PlayCameraShake(bDidShakeCamera);
}

void AAOCharacter::InitGAS()
{
}

void AAOCharacter::ClearGAS()
{
}

bool AAOCharacter::IsEnemy(AActor* TargetActor)
{
	UAbilitySystemComponent* MyASC = GetAbilitySystemComponent();
	UAbilitySystemComponent* TargetASC = Cast<AAOCharacter>(TargetActor)->GetAbilitySystemComponent();
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
		// gc

		//APlayerController* PC = GetWorld()->GetFirstPlayerController();
		//if (PC && CameraShakeClass)
		//{
		//	bDidShakeCamera = true;
		//	PC->ClientStartCameraShake(CameraShakeClass);
		//}
	}
}

void AAOCharacter::DrawDebugCapsuleCollider(const FVector& CapsuleOrigin, const float CapsuleHalfHeight, const float AttackRadius, const FColor DrawColor)
{
#if ENABLE_DRAW_DEBUG
	DrawDebugCapsule(GetWorld(), CapsuleOrigin, CapsuleHalfHeight, AttackRadius, FRotationMatrix::MakeFromZ(GetActorForwardVector()).ToQuat(), DrawColor, false, 1.0f);
#endif
}
