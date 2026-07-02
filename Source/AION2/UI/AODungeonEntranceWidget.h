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

/**
 *
 */
UCLASS()
class AION2_API UAODungeonEntranceWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetNotJoined();
	void SetDungeonInfo(const Protocol::DungeonInfo& DungeonInfo);
	void SetDungeonCreated(const Protocol::DungeonInfo& DungeonInfo);
	void SetDungeonEntered(int32 DungeonId, const Protocol::DungeonPlayerInfo& EnterPlayer);
	void SetDungeonReady(int32 DungeonId, uint64 PlayerId);

private:
	void ApplyEntranceState();

	void SetMemberSlots(const Protocol::DungeonInfo& DungeonInfo);

	// 멤버 하나의 Slot만 설정
	void SetMemberSlot(int32 SlotIndex, const Protocol::DungeonPlayerInfo& PlayerInfo);

	// 모든 목록을 지움
	void ClearMemberSlots();


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
	UFUNCTION()
	void OnEnterButtonClicked();

	UFUNCTION()
	void OnCreateButtonClicked();

	UFUNCTION()
	void OnStartButtonClicked();

	UFUNCTION()
	void OnReadyButtonClicked();

	UFUNCTION()
	void OnExitButtonClicked();

private:
	UAOPlayerManager* GetPlayerManager() const;
};
