// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/AOCharacter.h"
#include "AbilitySystemComponent.h"
#include "GameplayAbilitySpecHandle.h"
#include "GameplayTagContainer.h"
#include "AOMonsterBase.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAbilityFinishedEvent);

UCLASS()
class AION2_API AAOMonsterBase : public AAOCharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AAOMonsterBase(const FObjectInitializer& ObjectInitializer);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void PossessedBy(AController* NewController) override;

	virtual void InitGAS() override;
	virtual void ClearGAS() override;

	// Navigation Mesh의 범위 밖에 이동인지 아닌지를 판단하는 함수 
	UFUNCTION(BlueprintCallable, Category = "Navigation")
	bool CanMoveOnNavMesh(const FVector Direction, float Distance);


public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, Category = "Montage")
	UAnimMontage* GetMontageByTag(const FGameplayTag& MontageTag) const;

	UPROPERTY(BlueprintAssignable, Category = "GAS")
	FOnAbilityFinishedEvent OnAbilityFinishedEvent;


protected:
	// AttributeSet
	UPROPERTY(EditAnywhere, Category = "GAS", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UAOAttributeSet> AttributeSet;

	virtual void InitAttributeSet();
private:
	// GAS 관련 
	UPROPERTY()
	TArray<FGameplayAbilitySpecHandle> AbilityHandles;

	UPROPERTY(EditDefaultsOnly, Category = "Montage", meta = (AllowPrivateAccess = "true"))
	TMap<FGameplayTag, TObjectPtr<UAnimMontage>> MontageMap;

	// AODungeonGameMode -> TEST
	UFUNCTION(BlueprintCallable, Category = "Dungeon")
	void HandleBossDeath();

public:
	/*
	* 0 = 일반 몬스터
	* 1 = b1
	* 2 = b2
	* 3 = b3
	*/
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Dungeon")
	int32 DungeonBossIndex = 0;
	UPROPERTY(EditDefaultsOnly, Category = "GAS", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UDA_AbilitySet> AbilitySet;



	UFUNCTION(BlueprintCallable, Category = "Dungeon")
	void SetDungeonBossActive(bool bActive);

	void Die();

protected :
	void OnHealthChanged(const FOnAttributeChangeData& Data);

protected :
	FDelegateHandle HealthChangedDelegateHandle;
};
