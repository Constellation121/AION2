// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/AODungeonEntranceWidget.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
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
		CreateButton->OnClicked.AddDynamic(this, &UAODungeonEntranceWidget::OnCreateButtonClicked);
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

void UAODungeonEntranceWidget::SetLeaderClass(uint8 InLeaderClass)
{
	if (EnterButton)
	{
		EnterButton->SetVisibility(ESlateVisibility::Collapsed);
	}
	if (CreateButton)
	{
		CreateButton->SetVisibility(ESlateVisibility::Collapsed);
	}

	SetImage(LeaderClass, InLeaderClass);
}

void UAODungeonEntranceWidget::SetLeaderName(FString InLeaderName)
{
	LeaderName->SetText(FText::FromStringView(InLeaderName));
}

void UAODungeonEntranceWidget::SetMember1Class(uint8 InLeaderClass)
{

}

void UAODungeonEntranceWidget::SetMember1Name(FString InLeaderName)
{

}	

void UAODungeonEntranceWidget::SetImage(class UImage* TargetImage, uint8 ClassType)
{
	FSlateBrush Brush;
	switch (ClassType)
	{
	case 1:
	{
		Brush.SetResourceObject(AssassinImage);
		TargetImage->SetBrush(Brush);
		break;
	}
	case 2:
	{
		Brush.SetResourceObject(AssassinImage);
		TargetImage->SetBrush(Brush);
		break;
	}
	case 3:
	{
		Brush.SetResourceObject(AssassinImage);
		TargetImage->SetBrush(Brush);
		break;
	}
	case 4:
	{
		Brush.SetResourceObject(AssassinImage);
		TargetImage->SetBrush(Brush);
		break;
	}
	default :
	{
		UE_LOG(LogTemp, Warning, TEXT("Leader Class Empty"));
	}
	}
}