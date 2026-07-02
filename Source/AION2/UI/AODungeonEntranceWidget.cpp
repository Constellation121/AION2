// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/AODungeonEntranceWidget.h"
#include "Components/Button.h"
#include "Components/Overlay.h"
#include "Game/AOGameInstance.h"
#include "UI/AOClassSwitcherWidget.h"
#include "Manager/AOPlayerManager.h"
#include "AION2.h"

void UAODungeonEntranceWidget::NativeConstruct()
{
	Super::NativeConstruct();

	MemberClassSlots = {
	Member1,
	Member2,
	Member3,
	Member4
	};

	// EnterButton 이벤트 바인딩
	if (EnterButton)
	{
		EnterButton->OnClicked.RemoveAll(this);
		EnterButton->OnClicked.AddDynamic(this, &UAODungeonEntranceWidget::OnEnterButtonClicked);
	}

	// CreateButton 이벤트 바인딩
	if (CreateButton)
	{
		CreateButton->OnClicked.RemoveAll(this);
		CreateButton->OnClicked.AddDynamic(this, &UAODungeonEntranceWidget::OnCreateButtonClicked);
	}

	if (ReadyButton)
	{
		ReadyButton->OnClicked.RemoveAll(this);
		ReadyButton->OnClicked.AddDynamic(this, &UAODungeonEntranceWidget::OnReadyButtonClicked);
	}

	if (StartButton)
	{
		StartButton->OnClicked.RemoveAll(this);
		StartButton->OnClicked.AddDynamic(this, &UAODungeonEntranceWidget::OnStartButtonClicked);
	}

	if (ExitButton)
	{
		ExitButton->OnClicked.RemoveAll(this);
		ExitButton->OnClicked.AddDynamic(this, &UAODungeonEntranceWidget::OnExitButtonClicked);
	}

	// 처음에는 참가하지 않은 상태로 조정
	SetNotJoined();
}

void UAODungeonEntranceWidget::OnEnterButtonClicked()
{
	UE_LOG(LogTemp, Warning, TEXT("C++: Enter Button Clicked!"));

	Protocol::C_DungeonEnteracket  EnterPacket;

	if (const UAOGameInstance* GI = Cast<UAOGameInstance>(GetGameInstance()))
	{
		EnterPacket.set_playerid(GI->GetMyPlayerId());
	}

	SEND_PACKET(EnterPacket, PKT_C_DUNGEONENTER);
}

void UAODungeonEntranceWidget::OnCreateButtonClicked()
{
	UE_LOG(LogTemp, Warning, TEXT("C++: Create Button Clicked!"));

	Protocol::C_DungeonCreatePacket CreatePacket;

	SEND_PACKET(CreatePacket, PKT_C_DUNGEONCREATE);
	
}
void UAODungeonEntranceWidget::OnStartButtonClicked()
{
	UE_LOG(LogTemp, Warning, TEXT("C++: Create Button Clicked!"));

	const UAOPlayerManager* PlayerManager = GetPlayerManager();
	if (!PlayerManager)
	{
		return;
	}

	const FPlayerDungeonRoomState& State = PlayerManager->GetMyDungeonRoomState();
	if (!State.IsLeader() || State.DungeonId <= 0)
	{
		return;
	}


	Protocol::C_DungeonStartacket StartPacket;
	StartPacket.set_dungeonid(State.DungeonId);
	SEND_PACKET(StartPacket, PKT_C_DUNGEONSTART);	
}

void UAODungeonEntranceWidget::OnReadyButtonClicked()
{
	const UAOPlayerManager* PlayerManager = GetPlayerManager();
	if (!PlayerManager)
	{
		return;
	}

	const FPlayerDungeonRoomState& State = PlayerManager->GetMyDungeonRoomState();
	if (!State.IsMember() || State.DungeonId <= 0)
	{
		return;
	}

	Protocol::C_DungeonReadyacket ReadyPacket;
	ReadyPacket.set_dungeonid(State.DungeonId);
	SEND_PACKET(ReadyPacket, PKT_C_DUNGEONREADY);
}

void UAODungeonEntranceWidget::OnExitButtonClicked()
{

	// TODO: 서버에 방 나가기 패킷이 생기면 여기서 송신.
	// 지금은 로컬 상태를 먼저 Clear하면 서버 상태와 UI가 어긋날 수 있으니,
	// 서버 ack를 받은 뒤 PlayerManager->ClearMyDungeonRoomState() + SetNotJoined() 하는 편이 안전
	UE_LOG(LogTemp, Warning, TEXT("C++: Exit Button Clicked!"));

	if (ExitButton)
	{
		// Leader면 방 주인을 넘기기
		// 이후 방 나가기
	}
}

UAOPlayerManager* UAODungeonEntranceWidget::GetPlayerManager() const
{
	if (const UGameInstance* GI = GetGameInstance())
	{
		return GI->GetSubsystem<UAOPlayerManager>();
	}

	return nullptr;
}

void UAODungeonEntranceWidget::SetNotJoined()
{
	ClearMemberSlots();
	ApplyEntranceState();
}

void UAODungeonEntranceWidget::SetDungeonInfo(const Protocol::DungeonInfo& DungeonInfo)
{
	SetMemberSlots(DungeonInfo);
	ApplyEntranceState();
}


void UAODungeonEntranceWidget::SetDungeonCreated(const Protocol::DungeonInfo& DungeonInfo)
{
	SetDungeonInfo(DungeonInfo);
}

void UAODungeonEntranceWidget::SetDungeonEntered(int32 DungeonId, const Protocol::DungeonPlayerInfo& EnterPlayer)
{
	if (UAOPlayerManager* PlayerManager = GetPlayerManager())
	{
		if (PlayerManager->GetMyDungeonRoomState().DungeonId == DungeonId)
		{
			SetMemberSlot(EnterPlayer.index(), EnterPlayer);
		}
	}

	ApplyEntranceState();
}

void UAODungeonEntranceWidget::SetDungeonReady(int32 DungeonId, uint64 PlayerId)
{
	ApplyEntranceState();
}

void UAODungeonEntranceWidget::ApplyEntranceState()
{
	const UAOPlayerManager* PlayerManager = GetPlayerManager();
	const FPlayerDungeonRoomState State = PlayerManager
		? PlayerManager->GetMyDungeonRoomState()
		: FPlayerDungeonRoomState();

	const bool bNotJoined = !State.IsJoined();
	const bool bLeader = State.IsLeader();
	const bool bMember = State.IsMember();
	const bool bReady = State.ReadyState == EReadyState::Ready;

	if (Overlay_PlayerRoomState)
	{
		Overlay_PlayerRoomState->SetVisibility(
			bNotJoined ? ESlateVisibility::Hidden : ESlateVisibility::SelfHitTestInvisible
		);
	}

	if (Overlay_TitleAreaBottom)
	{
		Overlay_TitleAreaBottom->SetVisibility(
			bNotJoined ? ESlateVisibility::Hidden : ESlateVisibility::SelfHitTestInvisible
		);
	}

	if (Overlay_OutRoom)
	{
		Overlay_OutRoom->SetVisibility(
			bNotJoined ? ESlateVisibility::Visible : ESlateVisibility::Hidden
		);
	}

	if (Overlay_InRoom)
	{
		Overlay_InRoom->SetVisibility(
			bNotJoined ? ESlateVisibility::Hidden : ESlateVisibility::Visible
		);
	}

	if (Overlay_InRoom_Member)
	{
		Overlay_InRoom_Member->SetVisibility(
			bMember ? ESlateVisibility::Visible : ESlateVisibility::Collapsed
		);
	}

	if (Overlay_InRoom_Leader)
	{
		Overlay_InRoom_Leader->SetVisibility(
			bLeader ? ESlateVisibility::Visible : ESlateVisibility::Collapsed
		);
	}

}

void UAODungeonEntranceWidget::SetMemberSlots(const Protocol::DungeonInfo& DungeonInfo)
{
	ClearMemberSlots();

	if (DungeonInfo.has_leaderinfo())
	{
		SetMemberSlot(0, DungeonInfo.leaderinfo());
	}

	for (int32 Index = 0; Index < DungeonInfo.members_size(); ++Index)
	{
		const Protocol::DungeonPlayerInfo& MemberInfo = DungeonInfo.members(Index);
		SetMemberSlot(MemberInfo.index(), MemberInfo);
	}
}

void UAODungeonEntranceWidget::SetMemberSlot(int32 SlotIndex, const Protocol::DungeonPlayerInfo& PlayerInfo)
{
	if (!MemberClassSlots.IsValidIndex(SlotIndex))
	{
		return;
	}

	if (MemberClassSlots[SlotIndex])
	{
		MemberClassSlots[SlotIndex]->SetClassWidget(static_cast<uint8>(PlayerInfo.memberclass()));
		MemberClassSlots[SlotIndex]->SetLeaderState(SlotIndex == 0);
		MemberClassSlots[SlotIndex]->SetReadyState(PlayerInfo.isready());
		MemberClassSlots[SlotIndex]->SetPlayerName(
			FText::FromString(UTF8_TO_TCHAR(PlayerInfo.membername().c_str())
			)
		);
	}
}

void UAODungeonEntranceWidget::ClearMemberSlots()
{
	for (UAOClassSwitcherWidget* Slot : MemberClassSlots)
	{
		if (Slot)
		{
			Slot->SetClassWidget(0);
			Slot->SetLeaderState(false);
			Slot->SetReadyState(false);
		}
	}
}

