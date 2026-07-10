// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/DungeonClearWidget.h"
#include "Components/CanvasPanel.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Game/AODungeonGameMode.h"

void UDungeonClearWidget::NativeConstruct()
{
	if (Btn_Check)
		Btn_Check->OnClicked.AddUniqueDynamic(this, &UDungeonClearWidget::OnCheckClicked);
}

void UDungeonClearWidget::SetDungeonClearWidget(int32 Gold)
{
	if (GoldPrice)
	{
		GoldPrice->SetText(FText::AsNumber(Gold));
	}

	if(Clear1)
	{
		Clear1->SetVisibility(ESlateVisibility::Visible);
	}

	if (Clear2)
	{
		Clear2->SetVisibility(ESlateVisibility::Visible);
	}

	RemainingTime = 3;

	if (CountdownText)
	{
		CountdownText->SetText(FText::AsNumber(RemainingTime));
	}

	GetWorld()->GetTimerManager().SetTimer(
		ClearTimer,
		this,
		&UDungeonClearWidget::DungeonReward,
		1.0f,
		false
	);

}

void UDungeonClearWidget::DungeonReward()
{
	if (ClearPopup)
	{
		ClearPopup->SetVisibility(ESlateVisibility::Visible);
	}
}

void UDungeonClearWidget::UpdateCountdown()
{
	RemainingTime--;

	if (RemainingTime <= 0)
	{
		GetWorld()->GetTimerManager().ClearTimer(ClearTimer);

		DungeonReward();
		return;
	}

	if (CountdownText)
	{
		CountdownText->SetText(FText::AsNumber(RemainingTime));
	}
}

void UDungeonClearWidget::OnCheckClicked()
{
	AAODungeonGameMode* GameMode = Cast<AAODungeonGameMode>(GetWorld()->GetAuthGameMode());
	if (GameMode)
	{
		GameMode->SendDungeonCompleteRequest();
	}
}
