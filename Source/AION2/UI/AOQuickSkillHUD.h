// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/AOUserWidgetBase.h"
#include "AOQuickSkillHUD.generated.h"

class UAOSkillQuickSlotWidget;
class UHorizontalBox;
class UAOWidgetBase;
/**
 * 
 */
UCLASS()
class AION2_API UAOQuickSkillHUD : public UAOUserWidgetBase
{
	GENERATED_BODY()
	

protected:
	virtual void NativeConstruct() override;

protected:
	// === Skill Slots ===
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
	TObjectPtr<UAOSkillQuickSlotWidget> Skill_1;

	UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
	TObjectPtr<UAOSkillQuickSlotWidget> Skill_2;

	UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
	TObjectPtr<UAOSkillQuickSlotWidget> Skill_3;

	UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
	TObjectPtr<UAOSkillQuickSlotWidget> Skill_4;

	UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
	TObjectPtr<UAOSkillQuickSlotWidget> Skill_Q;

	UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
	TObjectPtr<UAOSkillQuickSlotWidget> Skill_E;

	UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
	TObjectPtr<UAOSkillQuickSlotWidget> Skill_R;

	UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
	TObjectPtr<UAOSkillQuickSlotWidget> Skill_T;


	// Binding된 위 Widget들을 편하게 관리하기 위해 Map에 넣음
	UPROPERTY()
	TMap<int32, TObjectPtr<UAOSkillQuickSlotWidget>> SkillSlotByAbilityID;

	// === Effect Area ===
	TObjectPtr<UHorizontalBox> EffectArea;


	TObjectPtr<UAOWidgetBase> EffectSlotBase;
	
	
};
