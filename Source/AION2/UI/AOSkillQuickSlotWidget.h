// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "UI/AOSlotWidgetBase.h"
#include "UI/AOQuickSkillHUD.h"

#include "Data/AOSkillSlotViewData.h"

#include "AOSkillQuickSlotWidget.generated.h"


class UBtton;
class UImage;
class UTextBlock;
class UUserWidget;

/**
 * [각 SkillSlot에 대한 Widget]
 * 표시 / 상태만 담당
 *  (이 Slot의 담당 AbilityID는 상위인 QuickSlotSkillHUD가 이미 알고 있으므로)
 * 
 * [이 Widget이 표시/가져야 할 것] => 여러 AbilityID를 등록 가능하도록.
 * => Icon, Level, CooldownTag, EffectWidgetClass, ASC(Bound)
 * 
 * [이 Widget이 시행해야 할 것]
 * 아이콘/레벨/쿨타임/이펙트 표시, 눌림 피드백 재생
 */


/*
* TODO(SuYeon): 최종적으로 수정해야할 것.
* SlotWidget이 자체적으로 ViewData를 갖지 않고, 하위 Widget으로 Skill Icon을 추가해서 돌아가도록 구현
*/

UCLASS()
class AION2_API UAOSkillQuickSlotWidget : public UAOSlotWidgetBase
{
	GENERATED_BODY()
	
protected:
	void NativeDestruct() override;

public:
	void AddSkillSlotViewData(const FAOSkillSlotViewData& InViewData);
	void ClearSkillSlotViewData();

	void SetCurrentSkillIndex(int32 NewIndex);
	FORCEINLINE int32 GetCurrentSkillIndex() { return CurrentSkillIndex; }
	const FAOSkillSlotViewData* GetCurrentSkillSlotViewData() const;

	void PlaySkillPressedFeedback();

	// 쿨타임 가진 Skill 발동 시 Effect 표시: TODO
	void StartCooldown(float RemainingTime, float Duration);

	// EffectWidget의 표시를 풀어줌: TODO
	void StopCooldown();


public:
	// Blueprint에서 구현하는 기능이므로, 초록줄이 뜬다고 해서 정의 만들면 빌드 에러남. 
	// C++ 내에서 호출은 할 수 있다.
	UFUNCTION(BlueprintImplementableEvent, Category = "SkillSlot")
	void BP_PlayPressedFeedback();

	UFUNCTION(BlueprintImplementableEvent, Category = "SkillSlot")
	void BP_StartCooldown(float RemainingTime, float Duration);

	UFUNCTION(BlueprintImplementableEvent, Category = "SkillSlot")
	void BP_StopCooldown();


private:
	void InitSkillSlot(const int32 InAbilityID);

	// Init the Icon
	void SetSkillIcon(UTexture2D* Icon);

	// Set Skill Level Text
	void SetSkillLevel(int32 InLevel);


protected:
	// ============= Widget Hierachy ============

	// Button은 상위 class를 물려받아 "SlotButton"으로 존재.

	UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
	TObjectPtr<UImage> SkillImage;

	UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
	TObjectPtr<UTextBlock> TB_SkillLevel;

protected:
	ESlotType slotType = ESlotType::Skill_Quick;
	

private:
	// ============= Skill View Data ============
	UPROPERTY()
	TMap<int32, FAOSkillSlotViewData> ViewDataByAbilityID;

	UPROPERTY()
	int32 CurrentSkillIndex = INDEX_NONE;

	//매번 인자를 받기보다, 내부 멤버를 쓰도록 함: Play Effect용 private member.
private:
	// ============= Cool down ============
	// FGameplayTag는 GC 대상이 아니기 때문에 일반 private으로 둬도 된다. => 나중에 더 알아보기
	FGameplayTag CurrentCooldownTag;

	UPROPERTY()
	TSubclassOf<UUserWidget> CurrentEffectWidgetClass;

	FTimerHandle CooldownTimerHandle;

};
