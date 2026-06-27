// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Engine/DataTable.h"
#include "AOItemDataBase.generated.h"

USTRUCT(BlueprintType)
struct FItemData : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item")
	int32 ItemId = 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item")
	FText ItemName;

	// 아이템 사진
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item")
	UTexture2D* ItemIcon;

	// 최대 중첩 개수
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item")
	int32 MaxStack = 99;

	// 회복량
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item")
	int32 HealAcount = 50;

	// slot 위치
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item")
	int32 ItemSlot;
};



UCLASS(BlueprintType)
class AION2_API UAOItemDataBase : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
	// 아이템 id 값을 key로 관리
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item")
	TMap<int32, FItemData> ItemMap;
};
