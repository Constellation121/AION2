// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/AOUserWidgetBase.h"
#include "Player/AOPlayerState.h"

void UAOUserWidgetBase::BindToPlayerState(AAOPlayerState* InPlayerState)
{
	BoundPlayerState = InPlayerState;
	BoundASC = nullptr;

	if (BoundPlayerState)
	{
		BoundASC = BoundPlayerState->GetAbilitySystemComponent();
	}
}
