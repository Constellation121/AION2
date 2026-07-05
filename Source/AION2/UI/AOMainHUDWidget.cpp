// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/AOMainHUDWidget.h"

#include "UI/AOPlayerHUDWidget.h"
#include "UI/AODungeonHUDWidget.h"

void UAOMainHUDWidget::BindToPlayerState(AAOPlayerState* InPlayerState)
{
	Super::BindToPlayerState(InPlayerState);

	if (PlayerHUDWidget)
	{
		PlayerHUDWidget->BindToPlayerState(InPlayerState);
	}

	if (DungeonHUDWidget)
	{
		DungeonHUDWidget->BindToPlayerState(InPlayerState);
	}
}

void UAOMainHUDWidget::SetBossHUDVisible()
{
	if (DungeonHUDWidget)
	{
		DungeonHUDWidget->SetBossHudVisible(ESlateVisibility::Visible);
	}
}

