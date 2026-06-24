#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "Interface/CombatInterface.h"
#include "AOCharacter.generated.h"

UCLASS(Abstract)
class AION2_API AAOCharacter : public ACharacter, public IAbilitySystemInterface, public ICombatInterface
{
	GENERATED_BODY()

public:
	AAOCharacter(const FObjectInitializer& ObjectInitializer);

public:
	virtual class UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_DrawDebugCapsuleCollider(const FVector& CapsuleOrigin, const float CapsuleHalfHeight, const float AttackRadius, const FColor DrawColor);

public:
	virtual void CheckAttackHit(const FAttackData& AttackData) override;
	virtual void OnAttackSucceeded(const FAttackData& AttackData, AActor* HitActor, const FHitResult& HitResult, bool& bDidShakeCamera);

protected:
	virtual void InitGAS();
	virtual void ClearGAS();

private:
	bool IsEnemy(AActor* TargetActor);
	void PlayCameraShake(bool& bDidShakeCamera);
	void DrawDebugCapsuleCollider(const FVector& CapsuleOrigin, const float CapsuleHalfHeight, const float AttackRadius, const FColor DrawColor);

protected:
	UPROPERTY(EditAnywhere, Category = "GAS", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UAbilitySystemComponent> ASC;
};