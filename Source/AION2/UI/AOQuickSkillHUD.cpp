// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/AOQuickSkillHUD.h"
#include "UI/AOSkillQuickSlotWidget.h"

#include "Data/DA_AbilitySet.h"

#include "Character/Daeva/Daeva.h"


void UAOQuickSkillHUD::BindToASC(UAbilitySystemComponent* InASC)
{
    UE_LOG(LogTemp, Warning, TEXT("QuickSkillHUD BindToASC ENTER. InASC=%s"),
        *GetNameSafe(InASC));

    Super::BindToASC(InASC);

    if (!BoundASC)
    {
        UE_LOG(LogTemp, Error, TEXT("QuickSkillHUD BoundASC is null after Super."));
        return;
    }


    // Exception Handling => Logging.

    const ADaeva* Daeva = Cast<ADaeva>(BoundASC->GetAvatarActor());
    if (!Daeva)
    {
        Daeva = Cast<ADaeva>(GetOwningPlayerPawn());
    }

    const UDA_AbilitySet* AbilitySet = Daeva ? Daeva->GetCombatAbilitySet() : nullptr;

    UE_LOG(LogTemp, Warning,
        TEXT("QuickSkillHUD: Avatar=%s, AvatarClass=%s, AbilitySet=%s, AbilityCount=%d"),
        *GetNameSafe(Daeva),
        Daeva ? *GetNameSafe(Daeva->GetClass()) : TEXT("None"),
        *GetNameSafe(AbilitySet),
        AbilitySet ? AbilitySet->GetAbilities().Num() : -1);





    UE_LOG(LogTemp, Warning, TEXT("QuickSkillHUD Bind success. Daeva=%s AbilitySet=%s SlotMapNum=%d"),
        *GetNameSafe(Daeva),
        *GetNameSafe(AbilitySet),
        SkillSlotByAbilityID.Num());


    for (const TPair<int32, TObjectPtr<UAOSkillQuickSlotWidget>>& Pair : SkillSlotByAbilityID)
    {
        const int32 AbilityID = Pair.Key;
        UAOSkillQuickSlotWidget* SlotWidget = Pair.Value;

        if (!SlotWidget)
        {
            continue;
        }

        FGAData AbilityData;
        if (!AbilitySet->GetAbilityDataByInputID(AbilityID, AbilityData))
        {
            continue;
        }

        SlotWidget->InitSkillSlot(AbilityData);
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
