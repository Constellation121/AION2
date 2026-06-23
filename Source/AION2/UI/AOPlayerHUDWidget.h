// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/AOUserWidgetBase.h"
#include "AbilitySystemComponent.h"

#include "AOPlayerHUDWidget.generated.h"

/**
 * 
 */
UCLASS()
class AION2_API UAOPlayerHUDWidget : public UAOUserWidgetBase
{
	GENERATED_BODY()
	
public:
    virtual void BindToPlayerState(AAOPlayerState* InPlayerState) override;

protected:
    // UI 소멸자.
    virtual void NativeDestruct() override;

protected:
    void HandleHealthChanged(const FOnAttributeChangeData& Data);
    void HandleMaxHealthChanged(const FOnAttributeChangeData& Data);

    void HandleManaChanged(const FOnAttributeChangeData& Data);
    void HandleMaxManaChanged(const FOnAttributeChangeData& Data);

    void HandleStaminaChanged(const FOnAttributeChangeData& Data);
    void HandleMaxStaminaChanged(const FOnAttributeChangeData& Data);

private:
    void BindASCDelegates();
    void UnbindASCDelegates();
    
    void BroadcastInitialAttributes();

protected:
    // BlueprintImplementableEvent: C++ Body 구현 불가능, Blueprint에서 구현.
    UFUNCTION(BlueprintImplementableEvent)
    void OnHealthChanged(float NewValue, float MaxValue);

    UFUNCTION(BlueprintImplementableEvent)
    void OnMaxHealthChanged(float NewValue, float MaxValue);

    UFUNCTION(BlueprintImplementableEvent)
    void OnManaChanged(float NewValue, float MaxValue);

    UFUNCTION(BlueprintImplementableEvent)
    void OnMaxManaChanged(float NewValue, float MaxValue);

    UFUNCTION(BlueprintImplementableEvent)
    void OnStaminaChanged(float NewValue, float MaxValue);

    UFUNCTION(BlueprintImplementableEvent)
    void OnMaxStaminaChanged(float NewValue, float MaxValue);

private:
    ///
    // 중복 Binding/해제 문제에 대한 대안.
    // 기존 Delegate을 저장하고, 재Binding마다 기존 Delegate 제거 후 bind.
    ///
    FDelegateHandle HealthChangedHandle;
    FDelegateHandle MaxHealthChangedHandle;

    FDelegateHandle ManaChangedHandle;
    FDelegateHandle MaxManaChangedHandle;

    FDelegateHandle StaminaChangedHandle;
    FDelegateHandle MaxStaminaChangedHandle;
	

};
