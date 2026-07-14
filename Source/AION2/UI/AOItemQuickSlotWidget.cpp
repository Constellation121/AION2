// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/AOItemQuickSlotWidget.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "TimerManager.h"

void UAOItemQuickSlotWidget::StartCooldown(float Duration)
{
	if (Duration <= 0.0f)
	{
		StopCooldown();
		return;
	}

	CooldownDuration = Duration;
	CooldownRemainingTime = Duration;

	if (Image_CooldownOverlay)
	{
		Image_CooldownOverlay->SetVisibility(ESlateVisibility::Visible);
		Image_CooldownOverlay->SetRenderOpacity(1.0f);
	}

	if (Text_CooldownCount)
	{
		Text_CooldownCount->SetVisibility(ESlateVisibility::Visible);
		Text_CooldownCount->SetText(FText::AsNumber(FMath::CeilToInt(CooldownRemainingTime)));
	}

	BP_OnCooldownStarted(Duration);

	if (GetWorld())
	{
		GetWorld()->GetTimerManager().ClearTimer(CooldownTimerHandle);
		GetWorld()->GetTimerManager().SetTimer(CooldownTimerHandle, this, &UAOItemQuickSlotWidget::UpdateCooldown, 0.05f, true);
	}
}

void UAOItemQuickSlotWidget::StopCooldown()
{
	if (GetWorld())
	{
		GetWorld()->GetTimerManager().ClearTimer(CooldownTimerHandle);
	}

	if (Image_CooldownOverlay)
	{
		Image_CooldownOverlay->SetVisibility(ESlateVisibility::Collapsed);
	}

	if (Text_CooldownCount)
	{
		Text_CooldownCount->SetVisibility(ESlateVisibility::Collapsed);
	}

	BP_OnCooldownFinished();
}

void UAOItemQuickSlotWidget::UpdateCooldown()
{
	CooldownRemainingTime -= 0.05f;

	if (CooldownRemainingTime <= 0.0f)
	{
		StopCooldown();
		return;
	}

	float Percent = CooldownRemainingTime / CooldownDuration;

	if (Image_CooldownOverlay)
	{
		Image_CooldownOverlay->SetRenderOpacity(Percent);
	}

	if (Text_CooldownCount)
	{
		Text_CooldownCount->SetText(FText::AsNumber(FMath::CeilToInt(CooldownRemainingTime)));
	}

	BP_OnCooldownUpdated(CooldownRemainingTime, Percent);
}




