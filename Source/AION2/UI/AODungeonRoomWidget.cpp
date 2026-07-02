// Fill out your copyright notice in the Description page of Project Settings.


#include "AODungeonRoomWidget.h"

#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/WidgetSwitcher.h"
#include "Components/Overlay.h"

#include "UI/AOClassSwitcherWidget.h"
#include "Game/AOGameInstance.h"
#include "AODungeonEntranceWidget.h"
#include "Manager/AOPlayerManager.h"



void UAODungeonRoomWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (JoinButton)
	{
		JoinButton->OnClicked.RemoveAll(this);
		JoinButton->OnClicked.AddDynamic(this, &UAODungeonRoomWidget::HandleJoinClicked);
	}

	// MemberClassSlot 초기화
	MemberClassSlots = {
	MemberClassSlot1,
	MemberClassSlot2,
	MemberClassSlot3
	};
}


void UAODungeonRoomWidget::SetDungeonInfo(const Protocol::DungeonInfo& InInfo)
{
	DungeonId = InInfo.dungeonid();

	const Protocol::DungeonPlayerInfo& LeaderInfo = InInfo.leaderinfo();

	if (TB_LeaderName)
	{
		TB_LeaderName->SetText(FText::FromString(UTF8_TO_TCHAR(LeaderInfo.membername().c_str())));
	}

	SetLeaderClassType(static_cast<uint8>(LeaderInfo.memberclass()));
	SetMemberClasses(InInfo);
	ApplyParticipationState(InInfo);
}



void UAODungeonRoomWidget::HandleJoinClicked()
{
	OnJoinRequested.Broadcast(DungeonId);
}

void UAODungeonRoomWidget::SetLeaderClassType(uint8 ClassType)
{
	// Class Type 위젯 바꿔주기
	LeaderClassSlot->SetClassWidget(ClassType);
}

void UAODungeonRoomWidget::AddMemberClass(const Protocol::DungeonPlayerInfo& MemberInfo)
{
	const int32 SlotIndex = MemberInfo.index() - 1;

	if (!MemberClassSlots.IsValidIndex(SlotIndex))
	{
		return;
	}

	if (MemberClassSlots[SlotIndex])
	{
		MemberClassSlots[SlotIndex]->SetClassWidget(
			static_cast<uint8>(MemberInfo.memberclass())
		);
	}
}

void UAODungeonRoomWidget::SetMemberClasses(const Protocol::DungeonInfo& DungeonInfo)
{
	for (UAOClassSwitcherWidget* Slot : MemberClassSlots)
	{
		if (Slot)
		{
			Slot->SetClassWidget(0);
		}
	}

	for (int32 Index = 0; Index < DungeonInfo.members_size(); ++Index)
	{
		AddMemberClass(DungeonInfo.members(Index));
	}
}

void UAODungeonRoomWidget::ApplyParticipationState(const Protocol::DungeonInfo& DungeonInfo)
{
	const UAOPlayerManager* PlayerManager = nullptr;

	if (const UGameInstance* GI = GetGameInstance())
	{
		PlayerManager = GI->GetSubsystem<UAOPlayerManager>();
	}

	const FPlayerDungeonRoomState State = PlayerManager
		? PlayerManager->GetMyDungeonRoomState()
		: FPlayerDungeonRoomState();

	const bool bIsMyRoom = State.IsJoined() && State.DungeonId == DungeonInfo.dungeonid();
	const bool bRecruiting = DungeonInfo.status() == Protocol::RoomStatus::RECRUITING;

	if (Image_PlayerJoined)
	{
		Image_PlayerJoined->SetVisibility(
			bIsMyRoom ? ESlateVisibility::Visible : ESlateVisibility::Collapsed
		);
	}

	if (Overlay_NotJoin)
	{
		Overlay_NotJoin->SetVisibility(
			bIsMyRoom ? ESlateVisibility::Hidden : ESlateVisibility::SelfHitTestInvisible
		);
	}

	if (TB_StatusText)
	{
		if (bIsMyRoom)
		{
			TB_StatusText->SetText(FText::FromString(TEXT("참가 중")));
		}
		else if (bRecruiting)
		{
			TB_StatusText->SetText(FText::FromString(TEXT("즉시 참가")));
		}
		else
		{
			TB_StatusText->SetText(FText::FromString(TEXT("진행중")));
		}
	}

	if (JoinButton)
	{
		JoinButton->SetIsEnabled(bRecruiting && !bIsMyRoom);
	}
}
