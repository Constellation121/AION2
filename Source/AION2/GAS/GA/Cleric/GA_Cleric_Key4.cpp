#include "GAS/GA/Cleric/GA_Cleric_Key4.h"
#include "Physics/Collision.h"
#include "Game/AODungeonGameMode.h"

void UGA_Cleric_Key4::OnCheckAttackHitEvent(FGameplayEventData Payload)
{
	if (!HasAuthority(&CurrentActivationInfo))
	{
		return;
	}

	UAbilitySystemComponent* SourceASC = GetAbilitySystemComponentFromActorInfo();
	if (!SourceASC)
	{
		return;
	}

	AAOCharacter* AOCharacter = Cast<AAOCharacter>(GetAvatarActorFromActorInfo());
	if (!AOCharacter)
	{
		return;
	}

	TArray<FHitResult> OutHitResults;

	const float AttackRange = AttackData.TraceData.Range;
	const float AttackRadius = AttackData.TraceData.Radius;

	FCollisionQueryParams Params(SCENE_QUERY_STAT(AttackTrace), false, AOCharacter);

	FVector SweepStart = AOCharacter->GetActorTransform().TransformPosition(AttackData.TraceData.StartOffset);
	FVector SweepEnd = SweepStart + AttackData.TraceData.Direction.GetSafeNormal() * AttackRange;
	FVector CapsuleCenter = SweepStart + (SweepEnd - SweepStart) * 0.5f;

	bool bHitDetected = GetWorld()->SweepMultiByChannel(OutHitResults, SweepStart, SweepEnd, FQuat::Identity, CCHANNEL_ATTACK, FCollisionShape::MakeSphere(AttackRadius), Params);

	if (!bHitDetected)
	{
		return;
	}

	AAOCharacter* NearestCharacter = nullptr;
	float NearestDistance = 123456789.0f;
	for (const FHitResult& HitResult : OutHitResults)
	{
		AAOCharacter* HitCharacter = Cast<AAOCharacter>(HitResult.GetActor());
		if (!IsValid(HitCharacter))
		{
			continue;
		}

		if (AOCharacter->IsEnemy(HitCharacter))
		{
			continue;
		}

		if (!HitCharacter->IsDead())
		{
			continue;
		}

		const float Distance = FVector::Distance(AOCharacter->GetActorLocation(), HitCharacter->GetActorLocation());
		if (Distance < NearestDistance)
		{
			NearestDistance = Distance;
			NearestCharacter = HitCharacter;
		}
	}

	if (IsValid(NearestCharacter))
	{
		APlayerController* PlayerController = Cast<APlayerController>(NearestCharacter->GetController());
		if (!PlayerController)
		{
			return;
		}

		AAODungeonGameMode* DungeonGameMode = GetWorld()->GetAuthGameMode<AAODungeonGameMode>();
		if (!DungeonGameMode)
		{
			return;
		}

		DungeonGameMode->NotifyPlayerRespawnImmediately(PlayerController);

		if (UAbilitySystemComponent* TargetASC = NearestCharacter->GetAbilitySystemComponent())
		{
			if (AttackData.HitGameplayCueTag.IsValid())
			{
				FGameplayCueParameters CueParams;
				CueParams.Instigator = NearestCharacter;
				CueParams.EffectCauser = NearestCharacter;
				TargetASC->ExecuteGameplayCue(AttackData.HitGameplayCueTag, CueParams);
			}
		}
	}
}
