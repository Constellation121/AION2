// Fill out your copyright notice in the Description page of Project Settings.


#include "AOInventoryComponent.h"


UAOInventoryComponent::UAOInventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UAOInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

	InventorySlots.SetNum(MaxInventorySize);
	for (int32 i = 0; i < MaxInventorySize; ++i)
	{
		InventorySlots[i].SlotIndex = i;
	}
}

void UAOInventoryComponent::InitializeInventory(int32 SlotIndex, int32 ItemTemplateId, int32 ItemInstancedId, int32 Count)
{
	if (!InventorySlots.IsValidIndex(SlotIndex))
	{
		return;
	}

	FAOSlotData& Slot = InventorySlots[SlotIndex];
	Slot.ItemTemplateId = ItemTemplateId;
	Slot.ItemInstancedId = ItemInstancedId;
	Slot.Count = Count;
	Slot.SlotIndex = SlotIndex;

	if (OnInventoryChanged.IsBound())
	{
		OnInventoryChanged.Broadcast();
	}
}

bool UAOInventoryComponent::GetItemDataFromSlot(int32 SlotIndex, FAOSlotData& OutSlotData, FItemData& OutTemplateData)
{
	if (!InventorySlots.IsValidIndex(SlotIndex))
	{
		return false;
	}

	const FAOSlotData& Slot = InventorySlots[SlotIndex];
	if (!Slot.IsValid())
	{
		return false;
	}

	UAOItemDataBase* DataBase = ItemDataBaseAsset.LoadSynchronous();
	if (!DataBase)
	{
		return false;
	}

	if (DataBase->FindItemTemplate(Slot.ItemTemplateId, OutTemplateData))
	{
		OutSlotData = Slot;
		return true;
	}

	return false;
}

bool UAOInventoryComponent::FindItemTemplateData(int32 ItemTemplateId, FItemData& OutTemplateData)
{
	UAOItemDataBase* DataBase = ItemDataBaseAsset.LoadSynchronous();
	if (DataBase)
	{
		return DataBase->FindItemTemplate(ItemTemplateId, OutTemplateData);
	}
	return false;
}
