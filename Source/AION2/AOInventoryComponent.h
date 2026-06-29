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
	UAOInventoryComponent();

protected:
	virtual void BeginPlay() override;

public:
	// 슬롯 초기화 및 업데이트 (서버 패킷 수신 시 호출)
	void InitializeInventory(int32 SlotIndex, int32 ItemTemplateId, int32 ItemInstancedId, int32 Count);

	// 슬롯에서 런타임 데이터 및 기획 템플릿 데이터 조회
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool GetItemDataFromSlot(int32 SlotIndex, FAOSlotData& OutSlotData, FItemData& OutTemplateData);

	// UI 갱신 델리게이트
	UPROPERTY(BlueprintAssignable, Category = "Item")
	FOnInventoryChanged OnInventoryChanged;

	// 인벤토리 최대 크기
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inventory")
	int32 MaxInventorySize = 9;

protected:
	// 아이템 데이터베이스 에셋
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inventory")
	TSoftObjectPtr<UAOItemDataBase> ItemDataBaseAsset;
public:
	bool FindItemTemplateData(int32 ItemTemplateId, FItemData& OutTemplateData);

private:
	// 인벤토리 슬롯 배열 (런타임 데이터)
	UPROPERTY()
	TArray<FAOSlotData> InventorySlots;
	
};
