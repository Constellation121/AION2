// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/AOSlotWidgetBase.h"
#include "Item/AOItemDataBase.h" 
#include "AOItemQuickSlotWidget.generated.h"

/**
 * 
 */

class UTextBlock;
class UImage;

UCLASS()
class AION2_API UAOItemQuickSlotWidget : public UAOSlotWidgetBase
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "QuickSlot")
	void UpdateQuickSlot(const FAOSlotData& SlotData, const FItemData& TemplateData);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "QuickSlot")
	void ClearQuickSlot(const int32 SlotIndex);

	UFUNCTION(BlueprintCallable, Category = "QuickSlot")
	void StartCooldown(float Duration);

	UFUNCTION(BlueprintCallable, Category = "QuickSlot")
	void StopCooldown();

protected:
	void UpdateCooldown();

protected:
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
	TObjectPtr<UTextBlock> TextBlock_ItemCount;

	UPROPERTY(meta = (BindWidget), BlueprintReadOnly, Category = "Slot")
	TObjectPtr<UImage> Image_ItemImage;

	// Cooldown UMG components
	UPROPERTY(meta = (BindWidgetOptional), BlueprintReadOnly, Category = "Slot|Cooldown")
	TObjectPtr<UImage> Image_CooldownOverlay;

	UPROPERTY(meta = (BindWidgetOptional), BlueprintReadOnly, Category = "Slot|Cooldown")
	TObjectPtr<UTextBlock> Text_CooldownCount;

protected:
	ESlotType slotType = ESlotType::Item_Quick;

	FTimerHandle CooldownTimerHandle;
	float CooldownDuration = 0.0f;
	float CooldownRemainingTime = 0.0f;

protected:
	UFUNCTION(BlueprintImplementableEvent, Category = "QuickSlot")
	void BP_OnCooldownStarted(float Duration);

	UFUNCTION(BlueprintImplementableEvent, Category = "QuickSlot")
	void BP_OnCooldownUpdated(float RemainingTime, float Percent);

	UFUNCTION(BlueprintImplementableEvent, Category = "QuickSlot")
	void BP_OnCooldownFinished();
};
