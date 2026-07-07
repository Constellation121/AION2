#pragma once

#include "CoreMinimal.h"
#include "GAS/GA/GA_Attack.h"
#include "GA_Cleric_KeyQ.generated.h"

UCLASS()
class AION2_API UGA_Cleric_KeyQ : public UGA_Attack
{
	GENERATED_BODY()

protected:
	virtual void OnCheckAttackHitEvent(FGameplayEventData Payload) override;
};
