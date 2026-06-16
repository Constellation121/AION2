#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "AOCharacter.generated.h"

class UAbilitySystemComponent;
class UAOAttributeSet;
class UGameplayEffect;

UCLASS(Abstract)
class AION2_API AAOCharacter : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	AAOCharacter(const FObjectInitializer& ObjectInitializer);

public:
	virtual class UAbilitySystemComponent* GetAbilitySystemComponent() const override;

protected:
	virtual void PossessedBy(AController* NewController) override;
	virtual void OnRep_PlayerState() override;

	virtual void InitGAS();
	virtual void InitializeDefaultAttributes();

	virtual void ApplyDashStaminaRegenEffect();

protected:
	UPROPERTY(EditAnywhere, Category = "GAS", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UAbilitySystemComponent> ASC;

	UPROPERTY(EditAnywhere, Category = "GAS", meta = (AllowPrivateAccess = "ture"))
	TObjectPtr<UAOAttributeSet> AttributeSet;

	UPROPERTY(EditAnywhere, Category = "GAS | Attribute", meta = (AllowPrivateAccess = "ture"))
	TSubclassOf<UGameplayEffect> DefaultAttributeEffect;

	UPROPERTY(EditAnywhere, Category = "GAS | Attribute", meta = (AllowPrivateAccess = "ture"))
	TSubclassOf<UGameplayEffect>DashStaminaRegenEffect;

};
