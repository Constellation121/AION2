// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Network/PacketHeader.h"
#include <unordered_map>
#include "AOPlayerManager.generated.h"

/**
 * 
 */
USTRUCT()
struct FPlayerInfo
{
	GENERATED_BODY()
public:
	int32 ClassType;
	FVector StartLocation;
};

UCLASS()
class AION2_API UAOPlayerManager : public UGameInstanceSubsystem
{
	GENERATED_BODY()
private:
	UAOPlayerManager();
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;	
	
public:
	void HandleLogin();
	void HandleSpawn(Protocol::PlayerState& Info);
	
private:
	FVector PlayerStartLocation = FVector(442.0f,-147.0f, 90.0f );

private:
	Protocol::PlayerInfo MyPlayerInfo;
	std::unordered_map<uint64, std::shared_ptr<class Daeva>> PlayerInfos;

	UPROPERTY(EditDefaultsOnly, Category = "Spawn")
	TMap<int32, TSubclassOf<APawn>> JobClassMap;
	
	class UAOGameInstance* GameInstance;
};
