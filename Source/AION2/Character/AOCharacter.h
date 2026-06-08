#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "AOCharacter.generated.h"

UCLASS()
class AION2_API AAOCharacter : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	AAOCharacter();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void PossessedBy(AController* NewController) override;
	virtual void OnRep_PlayerState() override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

public:
	virtual class UAbilitySystemComponent* GetAbilitySystemComponent() const override;

private:
	virtual void InitGAS();

private:
	UPROPERTY(EditAnywhere, Category = "GAS", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UAbilitySystemComponent> ASC;
};
