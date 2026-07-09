// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/AOSkillQuickSlotWidget.h"

#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"

#include "Data/AOSkillSlotViewData.h"

void UAOSkillQuickSlotWidget::NativeDestruct()
{
    // CooldownTimer에서 사용한 TimerHandle clear.
    if (UWorld* World = GetWorld())
    {
        World->GetTimerManager().ClearTimer(CooldownTimerHandle);
    }

    Super::NativeDestruct();
}

void UAOSkillQuickSlotWidget::AddSkillSlotViewData(const FAOSkillSlotViewData& InViewData)
{
    ViewDataByComboIndex.Add(InViewData);
}

void UAOSkillQuickSlotWidget::ClearSkillSlotViewData()
{
    ViewDataByComboIndex.Empty();
}

void UAOSkillQuickSlotWidget::SetCurrentSkillIndex(int32 InNewIndex)
{
    CurrentSkillIndex = InNewIndex;

    SetSkillIcon(ViewDataByComboIndex[CurrentSkillIndex].Icon);
    SetSkillLevel(ViewDataByComboIndex[CurrentSkillIndex].AbilityLevel);

}

void UAOSkillQuickSlotWidget::SetSkillIcon(UTexture2D* Icon)
{
    if (!SkillImage || !Icon)
    {
        return;
    }

    // 두 번째 인자는 기존 ImageBrush의 Size를 
    // 새로 배정된 Texture의 Size에 맞출 것인지를 묻는 것.
    if (Icon)
    {
        SkillImage->SetBrushFromTexture(Icon, false);
        SkillImage->SetOpacity(1.0f);
    }

    // SizeBox로 감싸둬도 괜찮다!
}

void UAOSkillQuickSlotWidget::SetSkillLevel(int32 InLevel)
{
    if (TB_SkillLevel)
    {
        TB_SkillLevel->SetText(
            FText::FromString(FString::Printf(TEXT("Lv%d"), InLevel)));
    }
}

const FAOSkillSlotViewData* UAOSkillQuickSlotWidget::GetCurrentSkillSlotViewData() const
{
    if (CurrentSkillIndex != INDEX_NONE && ViewDataByComboIndex.Num() > 1)
    {
        return nullptr;
    }
    return &ViewDataByComboIndex[CurrentSkillIndex];
}

void UAOSkillQuickSlotWidget::PlaySkillPressedFeedback()
{
    if (!SlotButton)
    {
        return;
    }

    BP_PlayPressedFeedback();
}

void UAOSkillQuickSlotWidget::HandleComboInput()
{
    if (CurrentSkillIndex + 1  < ViewDataByComboIndex.Num())
    {
        CurrentSkillIndex++;
    }
    else
    {
        CurrentSkillIndex = 0;
    }


    UE_LOG(LogTemp, Warning, TEXT("%d"), CurrentSkillIndex);
    SetCurrentSkillIndex(CurrentSkillIndex);
}

void UAOSkillQuickSlotWidget::ResetComboInput()
{
    if (ViewDataByComboIndex.Num() > 0)
    {
        SetCurrentSkillIndex(0);
    }
}

void UAOSkillQuickSlotWidget::StartCooldown(float RemainingTime, float Duration)
{
    if (RemainingTime <= 0.0f || Duration <= 0.0f)
    {
        return;
    }

    //CooldownEndTime = GetWorld()->GetTimeSeconds() + RemainingTime;
    //CooldownDuration = Duration;
    //
    //BP_StartCooldown(RemainingTime, Duration);
    //UpdateCooldownText();
    //
    //GetWorld()->GetTimerManager().SetTimer(
    //    CooldownTimerHandle,
    //    this,
    //    &ThisClass::UpdateCooldownText,
    //    0.1f,
    //    true
    //);
}

void UAOSkillQuickSlotWidget::StopCooldown()
{
    //if (GetWorld())
    //{
    //    GetWorld()->GetTimerManager().ClearTimer(CooldownTimerHandle);
    //}
    //
    //if (TB_CooldownRemaining)
    //{
    //    TB_CooldownRemaining->SetText(FText::GetEmpty());
    //    TB_CooldownRemaining->SetVisibility(ESlateVisibility::Collapsed);
    //}
    //
    //BP_StopCooldown();
}

