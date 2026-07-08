// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/AOQuickSkillHUD.h"
#include "UI/AOSkillQuickSlotWidget.h"

#include "Data/DA_AbilitySet.h"
#include "Data/AOSkillSlotViewData.h"

#include "Character/Daeva/Daeva.h"


void UAOQuickSkillHUD::BindToASC(UAbilitySystemComponent* InASC)
{
    Super::BindToASC(InASC);

    // Exception Handling
    if (!BoundASC)
    {        
        return;
    }
   
    const ADaeva* Daeva = Cast<ADaeva>(BoundASC->GetAvatarActor());
    if (!Daeva)
    {
        Daeva = Cast<ADaeva>(GetOwningPlayerPawn());
    }

    // 한 번 더 넣어줬는데 안되면 return;
    if (!Daeva)
    {
        UE_LOG(LogTemp, Warning, TEXT("UAOQuickSkillHUD::BindToASC Daeva is null"));
        return;
    }

    const UDA_AbilitySet* AbilitySet = Daeva->GetCombatAbilitySet();

    // Class Character Blueprint에 DA_CombatAbilitySet_{Class}을 할당하지 않은 경우.
    if (!AbilitySet)
    {
        // AbilitySet 안됐을 때 확인용
        UE_LOG(
            LogTemp, 
            Warning, 
            TEXT("UAOQuickSkillHUD::BindToASC...CharacterName: %s,  AbilitySetName: %s"), 
            *GetNameSafe(Daeva), *GetPathNameSafe(AbilitySet)
        );
    }

    // Button Init하는 부분 바꿔줌.
    InitSkillSlots(AbilitySet);
}

void UAOQuickSkillHUD::InitSkillSlots(const UDA_AbilitySet* InAbilitySet)
{
    for (const TPair<int32, TObjectPtr<UAOSkillQuickSlotWidget>>& Pair : SkillSlotByAbilityID)
    {
        const int32 AbilityID = Pair.Key;
        UAOSkillQuickSlotWidget* SlotWidget = Pair.Value;

        // 화면에 존재할 거라고 생각하지만, Exception Handling.
        if (!SlotWidget)
        {
            continue;
        }

        // 해당 클래스가 이 Key에 구현한 GA_Attack 정보가 없으면 return.
        FGAData AbilityData;
        if (!InAbilitySet->GetAbilityDataByInputID(AbilityID, AbilityData))
        {
            continue;
        }

        // ViewData를 만들어서 넣어줌
        FAOSkillSlotViewData ViewData;
        ViewData.AbilityID = AbilityData.AbilityID;
        ViewData.Icon = AbilityData.Icon;
        ViewData.AbilityLevel = AbilityData.AbilityLevel;
        ViewData.CooldownTag = AbilityData.CooldownTag;

        /*
        * TODO(SuYeon): 나중에.
        * SlotWidget은 이 정보를 바탕으로 다시 CoolTime 등에 대해 PlayerStatus 위에 띄운다.
        * 다른 class로 분리해서 PlayerStatus Widget에 넣은 뒤,
        * 해당 상위 Widget을 통해 소통하는 게 나을 듯.
        */
        // Init 하지 말고 해당 SlotWidget의 ViewData array에 Add.
        SlotWidget->InitSkillSlot(ViewData);
    }
}

void UAOQuickSkillHUD::NativeConstruct()
{
	Super::NativeConstruct();

    // 456
    SkillSlotByAbilityID.Add(static_cast<int32>(EAbilityID::LB_1), Skill_R);
    SkillSlotByAbilityID.Add(static_cast<int32>(EAbilityID::LB_2), Skill_R);
    SkillSlotByAbilityID.Add(static_cast<int32>(EAbilityID::LB_3), Skill_R);

    // 789
    SkillSlotByAbilityID.Add(static_cast<int32>(EAbilityID::RB_1), Skill_T);
    SkillSlotByAbilityID.Add(static_cast<int32>(EAbilityID::RB_2), Skill_T);
    SkillSlotByAbilityID.Add(static_cast<int32>(EAbilityID::RB_3), Skill_T);
    
    // 10 11 12 13
    SkillSlotByAbilityID.Add(static_cast<int32>(EAbilityID::Key1), Skill_1);
    SkillSlotByAbilityID.Add(static_cast<int32>(EAbilityID::Key2), Skill_2);
    SkillSlotByAbilityID.Add(static_cast<int32>(EAbilityID::Key3), Skill_3);
    SkillSlotByAbilityID.Add(static_cast<int32>(EAbilityID::Key4), Skill_4);
    
    // 14 15
    SkillSlotByAbilityID.Add(static_cast<int32>(EAbilityID::KeyQ), Skill_Q);
    SkillSlotByAbilityID.Add(static_cast<int32>(EAbilityID::KeyE), Skill_E);


}

void UAOQuickSkillHUD::HandleCooldownTagChanged(FGameplayTag CooldownTag, int32 NewCount)
{
    //if (NewCount > 0)
    //{
    //    FAOSkillSlotViewData Data;
    //    if (!BuildCooldownViewData(CooldownTag, Data))
    //    {
    //        return;
    //    }
    //
    //    if (UAOSkillQuickSlotWidget* SlotWidget = FindSlotByCooldownTag(CooldownTag))
    //    {
    //        SlotWidget->StartCooldown(Data.RemainingTime, Data.Duration);
    //    }
    //
    //    OnCooldownStarted.ExecuteIfBound(Data);
    //}
    //else
    //{
    //    if (UAOSkillQuickSlotWidget* SlotWidget = FindSlotByCooldownTag(CooldownTag))
    //    {
    //        SlotWidget->StopCooldown();
    //    }
    //
    //    OnCooldownStopped.ExecuteIfBound(CooldownTag);
    //}
}

void UAOQuickSkillHUD::PlaySkillPressedFeedback(int32 InputId)
{
    if (!SkillSlotByAbilityID.Contains(InputId))
    {
        // Invalid 입력이 들어옴: return.
        // Dash, Jump 등의 Non-Skill 입력이 있음!
        return;
    }

    // GAS Input으로 그냥 받았는데, AbilityID랑 다를 수도 있음. 
    // 지금 Slot은 AbilityID로 Mapping되어있으므로, 확인 차 Log 찍어보기.
    // => 같은 ID로 들어가고 있었으나, 한 번 눌러도 중복 입력이 좀 많이 들어가는 상태.
    UE_LOG(LogTemp, Warning, TEXT("Input ID: %d"), InputId);

    // 해당 Id의 Slot effect 재생하도록 하기.
    SkillSlotByAbilityID[InputId]->PlaySkillPressedFeedback();
}
