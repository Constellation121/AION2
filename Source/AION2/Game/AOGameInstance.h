// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#if PLATFORM_WINDOWS
#include "Windows/AllowWindowsPlatformTypes.h"
#endif

#pragma push_macro("check")
#undef check
#pragma push_macro("verify")
#undef verify

#include "Network/Protocol.pb.h"

#pragma pop_macro("verify")
#pragma pop_macro("check")

#if PLATFORM_WINDOWS
#include "Windows/HideWindowsPlatformTypes.h"
#endif
#include "AOGameInstance.generated.h"

/**
 *
 */
UCLASS()
class AION2_API UAOGameInstance : public UGameInstance
{
	GENERATED_BODY()
public:
	virtual void Init() override;

	void TryAsyncConnect(const FString& Ip, int32 Port);

	bool ConnectToServer(const FString& Ip, int32 Port);

	bool IsServerConnected();

	UFUNCTION(BlueprintCallable)
	void SendSignUpPacket(const FString& Id, const FString& Password, int32 ClassType);

	UFUNCTION(BlueprintCallable)
	void SendLoginPacket(const FString& Id, const FString& Password);

	void SendPacket(void* Packet, int32 PacketSize);
	void SendPacket(google::protobuf::Message& Pkt, uint16 PacketId);
	
	class UAONetworkSubsystem* GetNetworkManager() { return UNetworkManager; }

public:
	UPROPERTY(BlueprintReadWrite, Category = "UI")
	TObjectPtr<class UAOLoginUserWidget> LoginWidget;

	//소켓을 담을 변수
	FSocket* ClientSocket;
	class UAONetworkSubsystem* UNetworkManager;
};
