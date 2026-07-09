// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/AOUserWidgetBase.h"
#include "GameplayTagContainer.h"
#include "Delegates/Delegate.h"
#include "AOQuickSkillHUD.generated.h"

class UAOSkillQuickSlotWidget;
class UHorizontalBox;
class UAOWidgetBase;

class UDA_AbilitySet;

struct FAOSkillSlotViewData;


// Daeva의 어떤 스킬 Cooldown이 시작/초기화됐을 때 시행할 함수들을 묶을 Delegate.
DECLARE_DELEGATE_OneParam(FAOCooldownStartedDelegate, const FAOSkillSlotViewData&);
DECLARE_DELEGATE_OneParam(FAOCooldownStoppedDelegate, FGameplayTag);


/**
 * = PlayerHUDWidget 하위에서 =  
 * [Player의 각 Skill에 대해 시각적으로 반응하는 SkillSlot들을 소유 및 관리]
 * - 어떤 AbilityID가 어떤 Slot에 연결될지 && ASC/AbilitySet을 알고있음
 */
UCLASS()
class AION2_API UAOQuickSkillHUD : public UAOUserWidgetBase
{
	GENERATED_BODY()
	
public:
	// 상위 Widget에서 호출
	virtual void BindToASC(UAbilitySystemComponent* InASC) override;

protected:
	virtual void NativeConstruct() override;

public:
	// PlayerController로부터 타고 태려와 InputId에 맞는 slot에 전달할 함수.
	void PlaySkillPressedFeedback(int32 InputId);
	
	// cooldown tag 변경에 따라 실행될 함수.
	void HandleCooldownTagChanged(FGameplayTag CooldownTag, int32 NewCount);


	void SetCurrentAbilityForSlot(int32 SlotIndex, int32 AbilityID);
	void PlayPressedFeedbackByAbilityID(int32 AbilityID);


private:
	void InitSkillSlots(const UDA_AbilitySet* InAbilitySet);

public:
	FAOCooldownStartedDelegate OnCooldownStarted;
	FAOCooldownStoppedDelegate OnCooldownStopped;

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

	// 해당 Slot에서 현재 보여줄 Skill의 내부 Index. { SlotIndex : SkillID  }.
	// => Slot 내에서, viewData의 array를 들고 있도록 하고 입력을 받으면 변경.
	TMap<int32, int32> SkillSlotBySlotIndex;
	
	// === Effect Area ===
	TObjectPtr<UHorizontalBox> EffectArea;

	TObjectPtr<UAOWidgetBase> EffectSlotBase;
	

	UPROPERTY()
	TObjectPtr<const UDA_AbilitySet> BoundAbilitySet;
};
