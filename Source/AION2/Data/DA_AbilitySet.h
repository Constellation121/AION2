#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayAbilitySpecHandle.h"
#include "DA_AbilitySet.generated.h"

USTRUCT(BlueprintType)
struct FGAData
{
    GENERATED_BODY()

    UPROPERTY(EditDefaultsOnly)
    TSubclassOf<class UGameplayAbility> Ability = nullptr;

    UPROPERTY(EditDefaultsOnly)
    int32 AbilityLevel = 1;

    UPROPERTY(EditDefaultsOnly)
    int32 InputID = -1;
};

UCLASS()
class AION2_API UDA_AbilitySet : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
    UPROPERTY(EditDefaultsOnly)
    TArray<FGAData> Abilities;

    void GiveToASC(class UAbilitySystemComponent* ASC, TArray<FGameplayAbilitySpecHandle>& OutHandles) const;
};
