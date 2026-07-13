// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interface/CombatInterface.h"
#include "LutalisElectricShockZone.generated.h"

class AAOCharacter;
class ADaeva;
class UMaterialInstanceDynamic;
class UMaterialInterface;
class UNiagaraComponent;
class UNiagaraSystem;
class USceneComponent;
class USphereComponent;
class UStaticMeshComponent;

UENUM()
enum class ELutalisElectricShockZoneVisualState : uint8
{
	Hidden,
	Warning,
	Strike
};

UCLASS(Blueprintable)
class AION2_API ALutalisElectricShockZone : public AActor
{
	GENERATED_BODY()

public:
	ALutalisElectricShockZone();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(BlueprintCallable, Category = "Lutalis|ElectricShock")
	void InitZone(AAOCharacter* InDamageCauser, const FAttackData& InDamageData, float InRadius);

	UFUNCTION(BlueprintCallable, Category = "Lutalis|ElectricShock")
	void StartWarning(float InWarningDuration);

	UFUNCTION(BlueprintCallable, Category = "Lutalis|ElectricShock")
	void ActivateStrike();

protected:
	UFUNCTION()
	void OnRep_VisualState();

	UFUNCTION()
	void OnRep_Radius();

	void StartWarningInternal(float InWarningDuration);
	void ActivateStrikeInternal();
	void ApplyZoneDimensions();
	void UpdateWarning(float DeltaTime);
	void ApplyStrikeDamage();
	void BuildDamageHitResult(ADaeva* Target, FHitResult& OutHitResult) const;
	void SetWarningVisible(bool bVisible);
	void SetStrikeVisible(bool bVisible);
	void RefreshTickEnabled();
	void DrawDamageDebug() const;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "ElectricShock")
	TObjectPtr<USceneComponent> SceneRoot;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "ElectricShock")
	TObjectPtr<UStaticMeshComponent> WarningMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "ElectricShock")
	TObjectPtr<UNiagaraComponent> StrikeNiagaraComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "ElectricShock")
	TObjectPtr<USphereComponent> DamageSphere;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ElectricShock|Visual")
	TObjectPtr<UMaterialInterface> WarningMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ElectricShock|Visual")
	TObjectPtr<UNiagaraSystem> StrikeNiagaraSystem;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ElectricShock|Visual")
	FVector StrikeEffectRelativeLocation = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ElectricShock|Visual")
	FRotator StrikeEffectRelativeRotation = FRotator::ZeroRotator;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ElectricShock|Visual")
	FName FillParameterName = TEXT("FillInnerArc");

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ElectricShock|Visual", meta = (ClampMin = "0.0"))
	float WarningMeshBaseDiameter = 100.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ElectricShock|Visual", meta = (ClampMin = "0.0"))
	float StrikeLifeTime = 1.2f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ElectricShock|Debug")
	bool bDrawDamageDebug = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ElectricShock|Debug", meta = (EditCondition = "bDrawDamageDebug", ClampMin = "0.0"))
	float DamageDebugDrawDuration = 0.25f;

private:
	UPROPERTY()
	TObjectPtr<AAOCharacter> DamageCauser;

	UPROPERTY()
	TObjectPtr<UMaterialInstanceDynamic> WarningMID;

	FAttackData DamageData;

	UPROPERTY(ReplicatedUsing = OnRep_Radius)
	float Radius = 350.f;

	UPROPERTY(ReplicatedUsing = OnRep_VisualState)
	ELutalisElectricShockZoneVisualState VisualState = ELutalisElectricShockZoneVisualState::Hidden;

	UPROPERTY(Replicated)
	float ReplicatedWarningDuration = 1.f;

	float WarningDuration = 1.f;
	float WarningElapsed = 0.f;
	bool bWarning = false;

	UPROPERTY()
	TSet<TObjectPtr<AActor>> DamagedActors;
};
