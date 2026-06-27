// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Item/AOItemDataBase.h"
#include "AOInventoryComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInventoryChanged);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class AION2_API UAOInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UAOInventoryComponent();

	// 서버와 아이템 동기화
	void SyncronizeInvectorySlot(int32 SlotIndex, int32 ItemId, int32 Count);

	// 슬롯 인덱스로 아이템 세부 정보 가져옴
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool GetItemDataFromSlot(int32 SlotIndex, FItemData& OutItemData);

	// UI 슬롯 새로고침 델리게이트
	UPROPERTY(EditAnywhere, Category = "Item")
	FOnInventoryChanged OnInventoryChanged;
protected:
	// 기획 데이터 에셋의 소프트 레퍼런스 (초기 로딩 시간 및 메모리 최적화)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inventory")
	TSoftObjectPtr<UAOItemDataBase> ItemDataBaseAsset;

private:
	// 인벤토리 최대 슬롯 크기 고정 
	UPROPERTY()
	TArray<FItemData> InventorySlots;
	
};
