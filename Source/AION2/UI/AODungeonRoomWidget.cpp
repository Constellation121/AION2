// Fill out your copyright notice in the Description page of Project Settings.


#include "AODungeonRoomWidget.h"

#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/WidgetSwitcher.h"
#include "Components/Overlay.h"

#include "UI/AOClassSwitcherWidget.h"
#include "Game/AOGameInstance.h"




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

	if (TB_StatusText)
	{
		const bool bRecruiting = InInfo.status() == Protocol::RoomStatus::RECRUITING;
		TB_StatusText->SetText(bRecruiting ? FText::FromString(TEXT("즉시 참가")) : FText::FromString(TEXT("진행중")));
	}

	SetLeaderClassType(static_cast<uint8>(LeaderInfo.memberclass()));

	if (JoinButton)
	{
		JoinButton->SetIsEnabled(InInfo.status() == Protocol::RoomStatus::RECRUITING);
	}
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
	const int32 SlotCount = MemberClassSlots.Num();
	const int32 MemberCount = FMath::Min(DungeonInfo.members_size(), SlotCount);

	for (int32 Index = 0; Index < MemberCount; ++Index)
	{
		if (!MemberClassSlots[Index])
		{
			continue;
		}

		const Protocol::DungeonPlayerInfo& MemberInfo = DungeonInfo.members(Index);
		MemberClassSlots[Index]->SetClassWidget(
			static_cast<uint8>(MemberInfo.memberclass())
		);
	}

	for (int32 Index = MemberCount; Index < SlotCount; ++Index)
	{
		if (MemberClassSlots[Index])
		{
			MemberClassSlots[Index]->SetClassWidget(0); // Empty or ALL
		}
	}
}

void UAODungeonRoomWidget::ApplyParticipationState(const Protocol::DungeonInfo& DungeonInfo)
{

	const bool bIsLeader = IsLeaderOfRoom(DungeonInfo);
	const bool bIsJoinedRoom = IsMyDungeonRoom(DungeonInfo);
	const bool bRecruiting = DungeonInfo.status() == Protocol::RoomStatus::RECRUITING;

	if (Image_PlayerJoined)
	{
		Image_PlayerJoined->SetVisibility(
			bIsJoinedRoom ? ESlateVisibility::SelfHitTestInvisible : ESlateVisibility::Collapsed
		);
	}

	if (Overlay_NotJoin)
	{
		Overlay_NotJoin->SetVisibility(
			bIsJoinedRoom ? ESlateVisibility::SelfHitTestInvisible : ESlateVisibility::Collapsed
		);
	}

	if (TB_StatusText)
	{
		if (bIsLeader || bIsJoinedRoom)
		{
			TB_StatusText->SetText(FText::FromString(TEXT("참가 중")));
		}
		else if (!bRecruiting)
		{
			TB_StatusText->SetText(FText::FromString(TEXT("진행 중")));
		}
		else
		{
			TB_StatusText->SetText(FText::FromString(TEXT("즉시 참가")));
		}
	}
}

bool UAODungeonRoomWidget::IsMyDungeonRoom(const Protocol::DungeonInfo& DungeonInfo) const
{
	const UAOGameInstance* GI = Cast<UAOGameInstance>(GetWorld()->GetGameInstance());
	if (!GI)
	{
		return false;
	}

	const uint64 MyPlayerId = GI->GetMyPlayerId();

	if (DungeonInfo.has_leaderinfo() && DungeonInfo.leaderinfo().memberid() == MyPlayerId)
	{
		return true;
	}

	for (int32 Index = 0; Index < DungeonInfo.members_size(); ++Index)
	{
		if (DungeonInfo.members(Index).memberid() == MyPlayerId)
		{
			return true;
		}
	}

	return false;
}

bool UAODungeonRoomWidget::IsLeaderOfRoom(const Protocol::DungeonInfo& DungeonInfo) const
{
	const UAOGameInstance* GI = Cast<UAOGameInstance>(GetWorld()->GetGameInstance());
	if (!GI)
	{
		return false;
	}


	return DungeonInfo.has_leaderinfo()
		&& DungeonInfo.leaderinfo().memberid() == GI->GetMyPlayerId();
}

