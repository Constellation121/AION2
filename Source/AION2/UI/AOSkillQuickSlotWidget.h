// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/AOSlotWidgetBase.h"
#include "AOSkillQuickSlotWidget.generated.h"

/**
 * 
 */

class UBtton;
class UImage;
class UTextBlock;

UCLASS()
class AION2_API UAOSkillQuickSlotWidget : public UAOSlotWidgetBase
{
	GENERATED_BODY()
	
public:
	// Init the Icon
	void SetSkillIcon(UTexture2D* Icon);

	// Set Skill Level Text
	void SetSkillLevel(int32 InLevel);

	// Pressed Feedback (Not Effect)
	void PlayPressedFeedback();


	void StartCooldown(float RemainingTime, float Duration);
	void StopCooldown();
	
	
	void ShowEffectWidget(TSubclassOf<UUserWidget> EffectWidgetClass);


protected:
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
	TObjectPtr<UButton> SkillButton;

	UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
	TObjectPtr<UImage> SkillImage;

	UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
	TObjectPtr<UTextBlock> TB_SkillLevel;

protected:
	ESlotType slotType = ESlotType::Skill_Quick;
	
	
};
