#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "Character/Daeva/Daeva.h"
#include "GA_Attack.generated.h"

USTRUCT(BlueprintType)
struct FTraceData
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly)
	float Range = 0.0f;

	UPROPERTY(EditDefaultsOnly)
	float Radius = 0.0f;

	UPROPERTY(EditDefaultsOnly)
	FVector StartOffset = FVector::ZeroVector;

	UPROPERTY(EditDefaultsOnly)
	FVector Direction = FVector::ZeroVector;
};

USTRUCT(BlueprintType)
struct FAttackData
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly)
	float DamageMultiplier = 1.f;

	//UPROPERTY(EditDefaultsOnly)
	//FGameplayTag HitGameplayCueTag;

	UPROPERTY(EditDefaultsOnly)
	FTraceData TraceData;
};

UCLASS()
class AION2_API UGA_Attack : public UGameplayAbility
{
	GENERATED_BODY()

public:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

protected:
	UFUNCTION()
	void OnMontageTaskFinished();

	UFUNCTION()
	void OnMontageTaskCancelled();

private:
	UPROPERTY(EditDefaultsOnly, Category = "AttackData", meta = (AllowPrivateAccess = "true"))
	FAttackData AttackData;

private:
	UPROPERTY(EditDefaultsOnly, Category = "Effects", meta = (AllowPrivateAccess = "true"))
	TArray<TSubclassOf<UGameplayEffect>> GameplayEffectsToApply;

	UPROPERTY(EditDefaultsOnly, Category = "Effects", meta = (AllowPrivateAccess = "true"))
	FGameplayTagContainer RemoveTagsOnActivate;

private:
	UPROPERTY(EditDefaultsOnly, Category = "Montage", meta = (AllowPrivateAccess = "true"))
	EMontageID MontageIDToPlay;

	UPROPERTY(EditDefaultsOnly, Category = "Montage", meta = (AllowPrivateAccess = "true"))
	float MontagePlayRate;

	UPROPERTY(EditDefaultsOnly, Category = "Montage", meta = (AllowPrivateAccess = "true"))
	FName StartSectionName;
};
