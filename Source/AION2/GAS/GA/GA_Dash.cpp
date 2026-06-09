#include "GAS/GA/GA_Dash.h"
#include "Character/Daeva/Daeva.h"

#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "GameFramework/CharacterMovementComponent.h"

void UGA_Dash::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
    Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

    if (ACharacter* Character = Cast<ACharacter>(ActorInfo->AvatarActor.Get()))
    {
        if (Character->GetMovementComponent()->IsFalling() || Character->GetMovementComponent()->IsFlying())
        {
            EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
            return;
        }
    }

    if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
    {
        EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
        return;
    }

    ADaeva* Daeva = Cast<ADaeva>(ActorInfo->AvatarActor.Get());
    UAbilityTask_PlayMontageAndWait* MontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, Daeva->GetDashMontage(), 1.0f);

    MontageTask->OnCompleted.AddDynamic(this, &UGA_Dash::OnDashFinished);
    MontageTask->OnBlendOut.AddDynamic(this, &UGA_Dash::OnDashFinished);
    MontageTask->OnInterrupted.AddDynamic(this, &UGA_Dash::OnDashCancelled);
    MontageTask->OnCancelled.AddDynamic(this, &UGA_Dash::OnDashCancelled);

    MontageTask->ReadyForActivation();
}

void UGA_Dash::OnDashFinished()
{
    EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UGA_Dash::OnDashCancelled()
{
    EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}
