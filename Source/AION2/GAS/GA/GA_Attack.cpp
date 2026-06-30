#include "GAS/GA/GA_Attack.h"
#include "GAS/GA/AT/AT_RotateToTarget.h"
#include "GAS/AOGameplayTags.h"
#include "GAS/AttributeSet/AOAttributeSet.h"

#include "Character/Daeva/Daeva.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "AbilitySystemComponent.h"
#include "Animation/AnimMontage.h"
#include "GameplayEffect.h"

void UGA_Attack::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData
)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	UE_LOG(
		LogTemp,
		Warning,
		TEXT("[DamageTrace][GA_Attack][Activate][Enter] Ability=%s ActorInfo=%s Avatar=%s HasAuthority=%d"),
		*GetName(),
		ActorInfo ? TEXT("Valid") : TEXT("Null"),
		ActorInfo ? *GetNameSafe(ActorInfo->AvatarActor.Get()) : TEXT("None"),
		HasAuthority(&ActivationInfo) ? 1 : 0
	);

	if (!ActorInfo)
	{
		UE_LOG(LogTemp, Warning, TEXT("[DamageTrace][GA_Attack][Activate][Abort] Ability=%s Reason=ActorInfoNull"), *GetName());
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	ADaeva* Daeva = Cast<ADaeva>(ActorInfo->AvatarActor.Get());

	if (!Daeva)
	{
		UE_LOG(LogTemp, Warning, TEXT("[DamageTrace][GA_Attack][Activate][Abort] Ability=%s Avatar=%s Reason=DaevaCastFailed"), *GetName(), ActorInfo ? *GetNameSafe(ActorInfo->AvatarActor.Get()) : TEXT("None"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("[DamageTrace][GA_Attack][Activate][Avatar] Ability=%s Daeva=%s NetMode=%d HasAuthority=%d MontageID=%d"), *GetName(), *GetNameSafe(Daeva), static_cast<int32>(Daeva->GetNetMode()), Daeva->HasAuthority() ? 1 : 0, static_cast<int32>(MontageIDToPlay));

	UAnimMontage* AttackMontage = Daeva->GetMontageByID(MontageIDToPlay);

	if (!AttackMontage)
	{
		UE_LOG(LogTemp,Error,TEXT("[DamageTrace][GA_Attack][Activate][Abort] Montage is null. Ability=%s MontageID=%d"),*GetName(),static_cast<int32>(MontageIDToPlay));

		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}


	// 마나 비용 검사 및 차감
	if (ManaCost > 0.f)
	{
		UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();

		if (!ASC)
		{
			UE_LOG(LogTemp, Error, TEXT("[DamageTrace][GA_Attack][Mana][Abort] ASC is null. Ability=%s"), *GetName());

			EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
			return;
		}

		const float CurrentMana =
			ASC->GetNumericAttribute(UAOAttributeSet::GetManaAttribute());

		UE_LOG(LogTemp,Warning,TEXT("[Mana] Check | Ability=%s | Current=%.1f | Cost=%.1f"),*GetName(),CurrentMana,ManaCost);

		if (CurrentMana < ManaCost)
		{
			UE_LOG(	LogTemp,Warning,TEXT("[Mana] Not enough mana | Current=%.1f | Cost=%.1f"),CurrentMana,ManaCost);

			EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
			return;
		}

		if (!ManaCostEffect)
		{
			UE_LOG(	LogTemp,Error,TEXT("[Mana] ManaCostEffect is null. Ability=%s"),*GetName());

			EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
			return;
		}

		FGameplayEffectSpecHandle ManaCostSpec =
			MakeOutgoingGameplayEffectSpec(ManaCostEffect, 1.0f);

		if (!ManaCostSpec.IsValid())
		{
			UE_LOG(	LogTemp,Error,TEXT("[Mana] ManaCostSpec invalid. Effect=%s"),*GetNameSafe(ManaCostEffect));

			EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
			return;
		}

		const FGameplayTag SkillManaCost = FGameplayTag::RequestGameplayTag(FName("Data.ManaCost"));

		ManaCostSpec.Data->SetSetByCallerMagnitude(	SkillManaCost,-ManaCost);

		UE_LOG(	LogTemp,Warning,TEXT("[Mana] Apply Start | GE=%s | SetByCaller=%s | Value=%.1f"),*GetNameSafe(ManaCostEffect),*SkillManaCost.ToString(),-ManaCost);

		ApplyGameplayEffectSpecToOwner(	Handle,	ActorInfo,	ActivationInfo,	ManaCostSpec);

		const float ManaAfterApply =ASC->GetNumericAttribute(UAOAttributeSet::GetManaAttribute());
		
		UE_LOG(LogTemp,	Warning,TEXT("[Mana] Apply End | Before=%.1f | After=%.1f | Expected=%.1f"),CurrentMana,ManaAfterApply,	CurrentMana - ManaCost);
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

		FGameplayEffectSpecHandle SpecHandle = MakeOutgoingGameplayEffectSpec(GameplayEffect, 1.0f);

		if (!SpecHandle.IsValid())
		{
			continue;
		}

		/*
			GE_AttackSlow은 Granted Tag에 State.Attacking이 들어있다.
			그 GE만 Duration을 몽타주 길이로 설정한다.
		*/
		const UGameplayEffect* EffectCDO = GameplayEffect->GetDefaultObject<UGameplayEffect>();

		if (EffectCDO && EffectCDO->GetGrantedTags().HasTagExact(AttackingTag))
		{
			SpecHandle.Data->SetSetByCallerMagnitude(AttackSlowDurationTag, AttackDuration);

			UE_LOG(LogTemp,Log,TEXT("[AttackSlow] Duration=%.2f / GE=%s"),AttackDuration,*GameplayEffect->GetName());
		}

		ApplyGameplayEffectSpecToOwner(Handle,ActorInfo,ActivationInfo,SpecHandle);
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
	UAbilityTask_PlayMontageAndWait* MontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
			this,
			NAME_None,
			AttackMontage,
			SafePlayRate,
			StartSectionName
		);

	if (!MontageTask)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	MontageTask->OnCompleted.AddDynamic(this,&UGA_Attack::OnMontageTaskFinished);

	MontageTask->OnBlendOut.AddDynamic(this,&UGA_Attack::OnMontageTaskFinished);

	MontageTask->OnInterrupted.AddDynamic(this,&UGA_Attack::OnMontageTaskCancelled);

	MontageTask->OnCancelled.AddDynamic(this,&UGA_Attack::OnMontageTaskCancelled);

	MontageTask->ReadyForActivation();

	// 공격 타격 Notify 이벤트 대기
	UAbilityTask_WaitGameplayEvent* WaitHitCheckTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this,EVENT_CHECKATTACKHIT);

	if (WaitHitCheckTask)
	{
		UE_LOG(LogTemp, Warning, TEXT("[DamageTrace][GA_Attack][WaitGameplayEvent][Bind] Ability=%s Avatar=%s Tag=%s"), *GetName(), *GetNameSafe(Daeva), *EVENT_CHECKATTACKHIT.ToString());
		WaitHitCheckTask->EventReceived.AddDynamic(this,&UGA_Attack::OnCheckAttackHitEvent);

		WaitHitCheckTask->ReadyForActivation();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[DamageTrace][GA_Attack][WaitGameplayEvent][Abort] Ability=%s Reason=TaskNull"), *GetName());
	}

    UAT_RotateToTarget* RotateTask = UAT_RotateToTarget::RotateToTarget(this, AttackData.AvailableRange, 15.0f);
    RotateTask->ReadyForActivation();
}

void UGA_Attack::OnMontageTaskFinished()
{
	EndAbility(CurrentSpecHandle,CurrentActorInfo,CurrentActivationInfo,true,false);
}

void UGA_Attack::OnMontageTaskCancelled()
{
	EndAbility(	CurrentSpecHandle,CurrentActorInfo,CurrentActivationInfo,true,true);
}

void UGA_Attack::OnCheckAttackHitEvent(FGameplayEventData Payload)
{
	AActor* AvatarActor = GetAvatarActorFromActorInfo();
	UE_LOG(LogTemp, Warning, TEXT("[DamageTrace][GA_Attack][OnCheckAttackHitEvent][Enter] Ability=%s Avatar=%s HasAuthority=%d EventTag=%s"), *GetName(), *GetNameSafe(AvatarActor), HasAuthority(&CurrentActivationInfo) ? 1 : 0, *Payload.EventTag.ToString());

	if (!HasAuthority(&CurrentActivationInfo))
	{
		UE_LOG(LogTemp, Warning, TEXT("[DamageTrace][GA_Attack][OnCheckAttackHitEvent][Skip] Ability=%s Avatar=%s Reason=NoAuthority"), *GetName(), *GetNameSafe(AvatarActor));
		return;
	}

	AAOCharacter* AOCharacter =	Cast<AAOCharacter>(AvatarActor);

	if (!AOCharacter)
	{
		UE_LOG(LogTemp, Warning, TEXT("[DamageTrace][GA_Attack][OnCheckAttackHitEvent][Abort] Ability=%s Avatar=%s Reason=AOCharacterCastFailed"), *GetName(), *GetNameSafe(AvatarActor));
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("[DamageTrace][GA_Attack][OnCheckAttackHitEvent][CallCheckAttackHit] Ability=%s Actor=%s"), *GetName(), *GetNameSafe(AOCharacter));
	AOCharacter->CheckAttackHit(AttackData);
}
