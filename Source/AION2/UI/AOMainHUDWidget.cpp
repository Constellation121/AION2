// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/AOMainHUDWidget.h"

#include "UI/AOPlayerHUDWidget.h"
#include "UI/AODungeonHUDWidget.h"
#include "Character/Monster/AOMonsterBase.h"
#include "AbilitySystemComponent.h"


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

void UAOMainHUDWidget::SetBossHUDVisible(AAOMonsterBase* Boss)
{
	if (!DungeonHUDWidget || !Boss)
	{
		return;
	}

	DungeonHUDWidget->SetBossHUDVisible(ESlateVisibility::Visible, Boss);
}

void UAOMainHUDWidget::HideBossHUDOnly()
{
	if (DungeonHUDWidget)
	{
		DungeonHUDWidget->HideBossHUDOnly();
	}
}

void UAOMainHUDWidget::ClearBossHUD()
{
	if (DungeonHUDWidget)
	{
		DungeonHUDWidget->ClearBossHUD();
	}
}
