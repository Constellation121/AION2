#include "GAS/GA/GA_Attack.h"
#include "Character/Daeva/Daeva.h"

#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"

void UGA_Attack::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
    Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

    FGameplayEffectSpecHandle SpecHandle = MakeOutgoingGameplayEffectSpec(CombatStateEffect, 1.f);
    ApplyGameplayEffectSpecToOwner(Handle, ActorInfo, ActivationInfo, SpecHandle);

    ADaeva* Daeva = Cast<ADaeva>(ActorInfo->AvatarActor.Get());
    UAbilityTask_PlayMontageAndWait* MontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, Daeva->GetMontageByAbilityInputID(EMontageID::LB), 2.0f, TEXT("Combo1"));

    MontageTask->OnCompleted.AddDynamic(this, &UGA_Attack::OnMontageTaskFinished);
    MontageTask->OnBlendOut.AddDynamic(this, &UGA_Attack::OnMontageTaskFinished);
    MontageTask->OnInterrupted.AddDynamic(this, &UGA_Attack::OnMontageTaskCancelled);
    MontageTask->OnCancelled.AddDynamic(this, &UGA_Attack::OnMontageTaskCancelled);
    MontageTask->ReadyForActivation();
}

void UGA_Attack::OnMontageTaskFinished()
{
    EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UGA_Attack::OnMontageTaskCancelled()
{
    EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}
