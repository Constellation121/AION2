// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/AODungeonHUDWidget.h"
#include "UI/AOMonsterHUDWidget.h"
#include "Character/Monster/AOMonsterBase.h"


void UAODungeonHUDWidget::SetBossHUDVisible(ESlateVisibility InVisibility, AAOMonsterBase* Boss)
{
	if (!BossHUDWidget)
	{
		return;
	}

	BossHUDWidget->SetVisibility(InVisibility);

	if (Boss)
	{
		BossHUDWidget->SetMonsterIndex(Boss->DungeonBossIndex);
		BossHUDWidget->BindToASC(Boss->GetAbilitySystemComponent());
	}
}

void UAODungeonHUDWidget::HideBossHUDOnly()
{
	if (BossHUDWidget)
	{
		BossHUDWidget->SetVisibility(ESlateVisibility::Hidden);
	}
}

void UAODungeonHUDWidget::ClearBossHUD()
{
	if (!BossHUDWidget)
	{
		return;
	}

	BossHUDWidget->UnbindFromASC();
	BossHUDWidget->SetVisibility(ESlateVisibility::Hidden);
}

