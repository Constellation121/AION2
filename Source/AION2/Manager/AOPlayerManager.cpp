// Fill out your copyright notice in the Description page of Project Settings.


#include "AOPlayerManager.h"
#include "Kismet/GameplayStatics.h"
#include "Game/AOGameInstance.h"

UAOPlayerManager::UAOPlayerManager()
{
	static ConstructorHelpers::FClassFinder<APawn> AssassinClassRef(TEXT(""));
	if (AssassinClassRef.Succeeded())
	{
		JobClassMap.Add(1, AssassinClassRef.Class);
	}
}

void UAOPlayerManager::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	GameInstance = Cast<UAOGameInstance>(GetGameInstance());
}

void UAOPlayerManager::HandleLogin()
{
	// 애니메이션 넣기

	FString VillagePath = TEXT("/Game/Map/Village");
	UGameplayStatics::OpenLevel(this, *VillagePath);
}

void UAOPlayerManager::HandleSpawn(Protocol::PlayerState& Info)
{
	if (!GameInstance)
		return;
	if (GameInstance->GetMyPlayerId() == Info.playerinfo().playerid())
	{
		
	}
	
	else
	{
		
	}
}
