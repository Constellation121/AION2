#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GA_Assassin_LB_1.generated.h"

UCLASS()
class AION2_API UGA_Assassin_LB_1 : public UGameplayAbility
{
	GENERATED_BODY()

public:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

public:
	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	TSubclassOf<UGameplayEffect> CombatStateEffect;

protected:
	UFUNCTION()
	void OnMontageTaskFinished();

	UFUNCTION()
	void OnMontageTaskCancelled();
};
