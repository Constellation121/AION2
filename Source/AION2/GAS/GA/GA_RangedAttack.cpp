#include "GAS/GA/GA_RangedAttack.h"
#include "GAS/GA/AT/AT_RotateToTarget.h"
#include "GAS/AOGameplayTags.h"
#include "Character/Daeva/Daeva.h"

#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "AbilitySystemComponent.h"
#include "Animation/AnimMontage.h"
#include "GameplayEffect.h"

void UGA_RangedAttack::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData
)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	UE_LOG(LogTemp, Warning, TEXT("[DamageTrace][GA_RangedAttack][Activate][Enter] Ability=%s ActorInfo=%s Avatar=%s HasAuthority=%d"), *GetName(), ActorInfo ? TEXT("Valid") : TEXT("Null"), ActorInfo ? *GetNameSafe(ActorInfo->AvatarActor.Get()) : TEXT("None"), HasAuthority(&ActivationInfo) ? 1 : 0);

	if (!ActorInfo)
	{
		UE_LOG(LogTemp, Warning, TEXT("[DamageTrace][GA_RangedAttack][Activate][Abort] Ability=%s Reason=ActorInfoNull"), *GetName());
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	ADaeva* Daeva = Cast<ADaeva>(ActorInfo->AvatarActor.Get());
	if (!Daeva)
	{
		UE_LOG(LogTemp, Warning, TEXT("[DamageTrace][GA_RangedAttack][Activate][Abort] Ability=%s Avatar=%s Reason=DaevaCastFailed"), *GetName(), ActorInfo ? *GetNameSafe(ActorInfo->AvatarActor.Get()) : TEXT("None"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	AAOCharacter* Target = Daeva->GetCurrentTarget();
	const float TargetDistance = IsValid(Target) ? FVector::Distance(Target->GetActorLocation(), Daeva->GetActorLocation()) : -1.0f;
	UE_LOG(LogTemp, Warning, TEXT("[DamageTrace][GA_RangedAttack][Activate][TargetCheck] Ability=%s Daeva=%s Target=%s Distance=%.1f AvailableRange=%.1f NetMode=%d HasAuthority=%d"), *GetName(), *GetNameSafe(Daeva), *GetNameSafe(Target), TargetDistance, AttackData.AvailableRange, static_cast<int32>(Daeva->GetNetMode()), Daeva->HasAuthority() ? 1 : 0);
	if (!IsValid(Target) || TargetDistance > AttackData.AvailableRange)
	{
		UE_LOG(LogTemp, Warning, TEXT("[DamageTrace][GA_RangedAttack][Activate][Abort] Ability=%s Target=%s Distance=%.1f Reason=InvalidOrOutOfRange"), *GetName(), *GetNameSafe(Target), TargetDistance);
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	UAnimMontage* AttackMontage =
		Daeva->GetMontageByID(MontageIDToPlay);

	if (!AttackMontage)
	{
		UE_LOG(LogTemp,Error,TEXT("[DamageTrace][GA_RangedAttack][Activate][Abort] Montage is null. Ability=%s MontageID=%d"),*GetName(),static_cast<int32>(MontageIDToPlay));

		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	const float SafePlayRate = FMath::Max(MontagePlayRate, 0.01f);

	const float AttackDuration = AttackMontage->GetPlayLength() / SafePlayRate;

	const FGameplayTag AttackSlowDurationTag = FGameplayTag::RequestGameplayTag(FName("Data.AttackSlowDuration"));

	const FGameplayTag AttackingTag = FGameplayTag::RequestGameplayTag(FName("State.Attacking"));

	// GameplayEffectsToApply 안에 있는 모든 GE 적용
	for (const TSubclassOf<UGameplayEffect> GameplayEffect : GameplayEffectsToApply)
	{
		if (!GameplayEffect)
		{
			continue;
		}

		FGameplayEffectSpecHandle SpecHandle =
			MakeOutgoingGameplayEffectSpec(GameplayEffect, 1.0f);

		if (!SpecHandle.IsValid())
		{
			continue;
		}

		/*
			GE_AttackSlow은 Granted Tag에 State.Attacking이 들어있다.
			그 GE만 Duration을 몽타주 길이로 설정한다.
		*/
		const UGameplayEffect* EffectCDO =
			GameplayEffect->GetDefaultObject<UGameplayEffect>();

		if (EffectCDO &&
			EffectCDO->GetGrantedTags().HasTagExact(AttackingTag))
		{
			SpecHandle.Data->SetSetByCallerMagnitude(AttackSlowDurationTag,	AttackDuration);

			UE_LOG(LogTemp,	Log,TEXT("[AttackSlow] Duration=%.2f / GE=%s"),	AttackDuration,*GameplayEffect->GetName());
		}

		ApplyGameplayEffectSpecToOwner(Handle,ActorInfo,ActivationInfo,	SpecHandle);
	}

	// 공격 시작 시 Sprint GE 제거
	if (!RemoveTagsOnActivate.IsEmpty())
	{
		if (UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo())
		{
			ASC->RemoveActiveEffectsWithGrantedTags(RemoveTagsOnActivate);
		}
	}

	// 공격 몽타주 재생
	UAbilityTask_PlayMontageAndWait* MontageTask =
		UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this,NAME_None,AttackMontage,SafePlayRate,StartSectionName);

	if (!MontageTask)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	MontageTask->OnCompleted.AddDynamic(this,&UGA_RangedAttack::OnMontageTaskFinished);

	MontageTask->OnBlendOut.AddDynamic(this,&UGA_RangedAttack::OnMontageTaskFinished);

	MontageTask->OnInterrupted.AddDynamic(this,&UGA_RangedAttack::OnMontageTaskCancelled);

	MontageTask->OnCancelled.AddDynamic(this,&UGA_RangedAttack::OnMontageTaskCancelled);

	MontageTask->ReadyForActivation();

	// 투사체 스폰 Notify 이벤트 대기
	UAbilityTask_WaitGameplayEvent* WaitHitCheckTask =
		UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
			this,
			EVENT_CHECKATTACKHIT
		);

	if (WaitHitCheckTask)
	{
		UE_LOG(LogTemp, Warning, TEXT("[DamageTrace][GA_RangedAttack][WaitGameplayEvent][Bind] Ability=%s Avatar=%s Tag=%s"), *GetName(), *GetNameSafe(Daeva), *EVENT_CHECKATTACKHIT.ToString());
		WaitHitCheckTask->EventReceived.AddDynamic(this,&UGA_RangedAttack::OnLaunchProjectileEvent);

		WaitHitCheckTask->ReadyForActivation();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[DamageTrace][GA_RangedAttack][WaitGameplayEvent][Abort] Ability=%s Reason=TaskNull"), *GetName());
	}

	FVector Direction = Target->GetActorLocation() - Daeva->GetActorLocation();
	Direction.Z = 0.f;
	Daeva->SetActorRotation(Direction.Rotation());

	UAT_RotateToTarget* RotateTask = UAT_RotateToTarget::RotateToTarget(this, AttackData.AvailableRange, 15.0f);
	RotateTask->ReadyForActivation();
}

void UGA_RangedAttack::OnMontageTaskFinished()
{
	EndAbility(CurrentSpecHandle,CurrentActorInfo,CurrentActivationInfo,true,false);
}

void UGA_RangedAttack::OnMontageTaskCancelled()
{
	EndAbility(CurrentSpecHandle,CurrentActorInfo,CurrentActivationInfo,true,true);
}

void UGA_RangedAttack::OnLaunchProjectileEvent(FGameplayEventData Payload)
{
	AActor* AvatarActor = GetAvatarActorFromActorInfo();
	UE_LOG(LogTemp, Warning, TEXT("[DamageTrace][GA_RangedAttack][OnLaunchProjectileEvent][Enter] Ability=%s Avatar=%s HasAuthority=%d EventTag=%s ProjectileClass=%s"), *GetName(), *GetNameSafe(AvatarActor), HasAuthority(&CurrentActivationInfo) ? 1 : 0, *Payload.EventTag.ToString(), *GetNameSafe(ProjectileClass.Get()));

	if (!HasAuthority(&CurrentActivationInfo))
	{
		UE_LOG(LogTemp, Warning, TEXT("[DamageTrace][GA_RangedAttack][OnLaunchProjectileEvent][Skip] Ability=%s Avatar=%s Reason=NoAuthority"), *GetName(), *GetNameSafe(AvatarActor));
		return;
	}

	AAOCharacter* AOCharacter = Cast<AAOCharacter>(AvatarActor);
	if (!AOCharacter)
	{
		UE_LOG(LogTemp, Warning, TEXT("[DamageTrace][GA_RangedAttack][OnLaunchProjectileEvent][Abort] Ability=%s Avatar=%s Reason=AOCharacterCastFailed"), *GetName(), *GetNameSafe(AvatarActor));
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("[DamageTrace][GA_RangedAttack][OnLaunchProjectileEvent][CallSpawnProjectile] Ability=%s Actor=%s ProjectileClass=%s Socket=%s"), *GetName(), *GetNameSafe(AOCharacter), *GetNameSafe(ProjectileClass.Get()), *ProjectileSpawnSocket.ToString());
	AOCharacter->SpawnAttackProjectile(AttackData, ProjectileClass, ProjectileSpawnSocket);
}
