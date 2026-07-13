#pragma once

#include "CoreMinimal.h"
#include "AOTrapZone.h"
#include "Actor/AOProjectile.h"
#include "AOTrap.generated.h"

class UGameplayEffect;

UCLASS()
class AION2_API AAOTrap : public AAOProjectile
{
	GENERATED_BODY()

public:
	AAOTrap();

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void SpawnTrapOnOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

public:
	UPROPERTY(EditDefaultsOnly, blueprintReadOnly, Category = "Trap")
	TSubclassOf<AAOTrapZone> TrapZoneClass;

private:
	bool bTrapSpawned = false;
};
