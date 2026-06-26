// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/AODungeonEntranceWidget.h"
#include "Components/Button.h"
#include "Game/AOGameInstance.h"
#include "AION2.h"

void UAODungeonEntranceWidget::NativeConstruct()
{
	Super::NativeConstruct();
	// EnterButton 이벤트 바인딩
	if (EnterButton)
	{
		EnterButton->OnClicked.AddDynamic(this, &UAODungeonEntranceWidget::OnEnterButtonClicked);
	}

	// CreateButton 이벤트 바인딩
	if (CreateButton)
	{
		EnterButton->OnClicked.AddDynamic(this, &UAODungeonEntranceWidget::OnCreateButtonClicked);
	}
}

void UAODungeonEntranceWidget::OnEnterButtonClicked()
{
	UE_LOG(LogTemp, Warning, TEXT("C++: Enter Button Clicked!"));
	UAOGameInstance* GI = Cast<UAOGameInstance>(GetWorld()->GetGameInstance());

	if (!GI) return;
	uint64 PlayerId = GI->GetMyPlayerId();

	Protocol::C_DungeonEnteracket EnterPacket;
	EnterPacket.set_playerid(PlayerId);
	SEND_PACKET(EnterPacket, PKT_C_DUNGEONENTER);

}

void UAODungeonEntranceWidget::OnCreateButtonClicked()
{
	UE_LOG(LogTemp, Warning, TEXT("C++: Create Button Clicked!"));

	UAOGameInstance* GI = Cast<UAOGameInstance>(GetWorld()->GetGameInstance());

	if (!GI) return;
	uint64 PlayerId = GI->GetMyPlayerId();

	Protocol::C_DungeonCreatePacket CreatePacket;
	SEND_PACKET(CreatePacket, PKT_C_DUNGEONCREATE);

}
