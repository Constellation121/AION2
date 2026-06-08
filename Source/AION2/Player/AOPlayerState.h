#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "AbilitySystemInterface.h"
#include "AOPlayerState.generated.h"

UCLASS()
class AION2_API AAOPlayerState : public APlayerState, public IAbilitySystemInterface
{
	GENERATED_BODY()
	
public:
	AAOPlayerState();

public:
	virtual class UAbilitySystemComponent* GetAbilitySystemComponent() const override;

private:
	UPROPERTY(EditAnywhere, Category = "GAS", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UAbilitySystemComponent> ASC;
};
