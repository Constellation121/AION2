// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/AOSkillQuickSlotWidget.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"

void UAOSkillQuickSlotWidget::SetSkillIcon(UTexture2D* Icon)
{
    if (!SkillImage || !Icon)
    {
        return;
    }


	SkillImage->SetBrushFromTexture(Icon, true);
}

void UAOSkillQuickSlotWidget::SetSkillLevel(int32 InLevel)
{
    if (TB_SkillLevel)
    {
        TB_SkillLevel->SetText(
            FText::FromString(FString::Printf(TEXT("Lv%d"), InLevel)));
    }
}

void UAOSkillQuickSlotWidget::PlayPressedFeedback()
{
}

void UAOSkillQuickSlotWidget::StartCooldown(float RemainingTime, float Duration)
{
}

void UAOSkillQuickSlotWidget::StopCooldown()
{
}

void UAOSkillQuickSlotWidget::ShowEffectWidget(TSubclassOf<UUserWidget> EffectWidgetClass)
{
}
