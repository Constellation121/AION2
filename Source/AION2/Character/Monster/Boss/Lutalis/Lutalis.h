// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/Monster/AOMonsterBase.h"
#include "Lutalis.generated.h"

class ALutalisElectricZone;

/**
 * 
 */

UCLASS()
class AION2_API ALutalis : public AAOMonsterBase
{
	GENERATED_BODY()
	
public:

	ALutalis(const FObjectInitializer& ObjectInitializer);

	UFUNCTION(BlueprintCallable, Category = "Lutalis|ElectricZone")
	bool BeginElectricRangeWarning(float WarningDuration);

	UFUNCTION(BlueprintCallable, Category = "Lutalis|ElectricZone")
	bool ActivateElectricRangeDamage();

	UFUNCTION(BlueprintCallable, Category = "Lutalis|ElectricZone")
	void ConsumeElectricZoneRequestTag();

	UFUNCTION(BlueprintCallable, Category = "Lutalis|ElectricZone")
	bool PrepareElectricZoneCast();

	UFUNCTION(BlueprintCallable, Category = "Lutalis|ElectricZone")
	bool TickRotateToPreparedElectricZoneYaw(float DeltaTime);

	UFUNCTION(BlueprintCallable, Category = "Lutalis|ElectricZone")
	bool BeginPreparedElectricRangeWarning(float WarningDuration);

	UFUNCTION(BlueprintCallable, Category = "Lutalis|ElectricZone")
	void ResetPreparedElectricZoneCast();

protected:

	virtual void BeginPlay() override;
	virtual void InitAttributeSet() override;


private:
	bool ResolveElectricZoneActor();

private:

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Lutalis|ElectricZone", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<ALutalisElectricZone> ElectricZoneActor;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Lutalis|ElectricZone", meta = (AllowPrivateAccess = "true"))
	float ElectricZoneRadius = 3000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Lutalis|ElectricZone", meta = (AllowPrivateAccess = "true"))
	float ElectricZoneSectorAngle = 90.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Lutalis|ElectricZone", meta = (AllowPrivateAccess = "true"))
	FAttackData ElectricZoneTickDamageData;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Lutalis|ElectricZone", meta = (AllowPrivateAccess = "true"))
	float ElectricZoneRotateSpeed = 720.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Lutalis|ElectricZone", meta = (AllowPrivateAccess = "true"))
	float ElectricZoneRotateTolerance = 3.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Lutalis|ElectricZone", meta = (AllowPrivateAccess = "true"))
	float ElectricZoneFacingYawOffset = 180.f;

	int32 PreparedElectricZoneArcIndex = INDEX_NONE;
	float PreparedElectricZoneTargetYaw = 0.f;
};
