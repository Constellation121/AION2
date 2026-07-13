// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/AOMainHUDWidget.h"

#include "UI/AOPlayerHUDWidget.h"
#include "UI/AODungeonHUDWidget.h"
#include "Character/Monster/AOMonsterBase.h"
#include "Player/AOPlayerState.h"
#include "AbilitySystemComponent.h"
#include "Manager/AOPlayerManager.h"
#include "Game/AOGameInstance.h"


void UAOMainHUDWidget::BindToPlayerState(AAOPlayerState* InPlayerState)
{
	Super::BindToPlayerState(InPlayerState);

	if (PlayerHUDWidget)
	{
		PlayerHUDWidget->BindToPlayerState(InPlayerState);
		PlayerHUDWidget->ChangeClassIcon(InPlayerState->GetMyClass());
		
		FString PlayerName = InPlayerState->GetMyName();
		if (PlayerName.IsEmpty())
		{
			if (UAOPlayerManager* PlayerManager = GetGameInstance() ? GetGameInstance()->GetSubsystem<UAOPlayerManager>() : nullptr)
			{
				UAOGameInstance* GameInstance = Cast<UAOGameInstance>(GetGameInstance());
				uint64 MyPlayerId = GameInstance ? GameInstance->GetMyPlayerId() : 0;
				PlayerName = PlayerManager->GetPlayerNameById(MyPlayerId);
			}
		}
		PlayerHUDWidget->SetPlayerName(FText::FromString(PlayerName));
	}

	// TODO(suyeon): 던전 HUD에 각 파티원 Stat 추가
	if (DungeonHUDWidget)
	{
		DungeonHUDWidget->BindToPlayerState(InPlayerState);
	}
}

void UAOMainHUDWidget::ShowTargetMonsterHUD(AAOMonsterBase* InMonster)
{
	if (DungeonHUDWidget)
	{
		DungeonHUDWidget->ShowTargetMonsterHUD(InMonster);
	}
}

void UAOMainHUDWidget::HideTargetMonsterHUD()
{
	if (DungeonHUDWidget)
	{
		DungeonHUDWidget->HideTargetMonsterHUD();
	}
}

void UAOMainHUDWidget::PlaySkillPressedFeedback(int32 InputId)
{
	if (PlayerHUDWidget)
	{
		PlayerHUDWidget->PlaySkillPressedFeedback(InputId);
	}
}


void UAOMainHUDWidget::PlaySkillSucceedEffect(int32 InputId)
{
	if (PlayerHUDWidget)
	{
		PlayerHUDWidget->PlaySkillSucceedEffect(InputId);
	}
}

