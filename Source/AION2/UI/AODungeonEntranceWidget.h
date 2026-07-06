// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Network/PacketHeader.h"
#include "Types/DungeonRoomTypes.h"
#include "AODungeonEntranceWidget.generated.h"

class UButton;
class UOverlay;
class UAOClassSwitcherWidget;
class UAOPlayerManager;
class UAODungeonRoomWidget;
/**
 *
 */
UCLASS()
class AION2_API UAODungeonEntranceWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	/*
	* PacketHandler에서 호출할 수 있도록 추가
	* Server에서 /누군가 Action에 대한 packet/을 줬을 때,
	* 현재 내가 보고 있는/참여 중인 방이면 멤버 슬롯 UI를 갱신.
	*/
	void SetNotJoined();
	void SetDungeonInfo(const Protocol::DungeonInfo& DungeonInfo);
	void SetDungeonCreated(const Protocol::DungeonInfo& DungeonInfo);
	void SetDungeonEntered(int32 DungeonId, const Protocol::DungeonPlayerInfo& EnterPlayer);
	void SetDungeonReady(int32 DungeonId, uint64 PlayerId);
	void SetDungeonExit(int32 DungeonId, uint64 ExitPlayerId, const Protocol::DungeonInfo& DungeonInfo);
	// Utils/AODungeonEntrance에서 호출하는 초기화
	void InitializeWaitingRoom();

	/*
	*  DungeonRoomWidget의 OnJoinRequested에 Binding.
	*
	*/
	UFUNCTION()
	void RequestEnterDungeon(int32 DungeonId);


public:
	// PacketHandler에서 호출할 수 있도록 public으로 설정
	void ApplyEntranceState();

	// 방 목록 Refresh.
	void RefreshDungeonRooms(const google::protobuf::RepeatedPtrField<Protocol::DungeonInfo>& DungeonRooms
	);

	void ShowErrorMessage(Protocol::DungeonFailReason Reason);

private:
	void SetMemberSlots(const Protocol::DungeonInfo& DungeonInfo);

	// 멤버 하나의 Slot만 설정
	void SetMemberSlot(int32 SlotIndex, const Protocol::DungeonPlayerInfo& PlayerInfo);

	// 모든 목록을 지움
	void ClearMemberSlots();

	// 새로 들어오거나 재진입 시 방 목록을 초기화
	void ClearDungeonRooms();

protected:
	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> EnterButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> CreateButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> ReadyButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> StartButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> ExitButton;

	// Room에 들어가면 보이지 않게 될 Object들
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UOverlay> Overlay_OutRoom;

	// Room에 들어가면 보일 Object들
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UOverlay> Overlay_PlayerRoomState;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UOverlay> Overlay_TitleAreaBottom;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UOverlay> Overlay_InRoom;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UOverlay> Overlay_InRoom_Member;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UOverlay> Overlay_InRoom_Leader;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr <class UTextBlock> ErrorMessage;


	// Room에 들어가면 하단에 보일 멤버 목록
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UAOClassSwitcherWidget> Member1;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UAOClassSwitcherWidget> Member2;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UAOClassSwitcherWidget> Member3;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UAOClassSwitcherWidget> Member4;

	// 위에서 Bind된 MemberClassSlot Widget들을 편하게 관리하기 위해 Widget 생성자에서 Array로 묶음
	UPROPERTY()
	TArray<TObjectPtr<UAOClassSwitcherWidget>> MemberClassSlots;

private:
	// 들어갈 dungeonid, playerID를 넣어 Packet 전송
	UFUNCTION()
	void OnEnterButtonClicked();

	UFUNCTION()
	void OnCreateButtonClicked();

	// dungeonid를 넣어 Packet 전송
	UFUNCTION()
	void OnStartButtonClicked();

	/*
	* 들어온 dungeonid, 준비한 playerID를 넣어 Packet 전송
	* (어느 대기방에 반영해야 하는지 알아야 하므로)
	*/
	UFUNCTION()
	void OnReadyButtonClicked();

	UFUNCTION()
	void OnExitButtonClicked();


private:
	// 만들어줄 방
	static constexpr int32 MaxDungeonRoomCount = 4;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UAODungeonRoomWidget> WBP_DunzeonRoom_0;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UAODungeonRoomWidget> WBP_DunzeonRoom_1;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UAODungeonRoomWidget> WBP_DunzeonRoom_2;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UAODungeonRoomWidget> WBP_DunzeonRoom_3;

	UPROPERTY(Transient)
	TArray<TObjectPtr<UAODungeonRoomWidget>> DungeonRoomWidgets;

private:
	FTimerHandle ErrorMessageTimerHandle;

	void ClearErrorMessage();

private:
	UAOPlayerManager* GetPlayerManager() const;

	bool bIsEnter = false;
};
