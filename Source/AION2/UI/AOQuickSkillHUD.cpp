// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/AOQuickSkillHUD.h"

#include "Character/Daeva/Daeva.h"

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
