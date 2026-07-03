// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/AODungeonHUDWidget.h"
#include "UI/AOMonsterHUDWidget.h"

void UAODungeonHUDWidget::SetBossHudVisible(ESlateVisibility InVisibility)
{
	if (BossHUDWidget)
	{
		BossHUDWidget->SetVisibility(InVisibility);
	}
}
