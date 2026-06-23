// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "AOUserWidgetBase.generated.h"

/**
 * 
 */

class AAOPlayerState;
class UAbilitySystemComponent;

UCLASS()
class AION2_API UAOUserWidgetBase : public UUserWidget
{
	GENERATED_BODY()
	
	
public:
	/*
	* 공통 Bind/Unbind 흐름 제공
	* Server에서는 절대 생성되지 않도록 함.
	*/
	virtual void BindToPlayerState(AAOPlayerState* InPlayerState);

protected:
	// Playeer State 저장
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<AAOPlayerState> BoundPlayerState;

	// ASC 저장
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UAbilitySystemComponent> BoundASC;
	
};
