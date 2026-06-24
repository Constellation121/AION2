#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "AOPlayerController.generated.h"

extern TAutoConsoleVariable<int32> CVarDrawAttackTrace;

class UAOMainHUDWidget;

UENUM()
enum class EInputType : uint8
{
	Game,
	UI
};

UCLASS()
class AION2_API AAOPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	AAOPlayerController();

public:
	UFUNCTION(Server, Reliable)
	void Server_SetShowColliderDebug();

public:
	// UI
	// PlayerBase(Daeva)가 준비됐음을 알리면(OnRep_PlayerState) 실행될 함수.
	void HandlePawnASCReady();

protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;

private:
	void SetInputMappingContext(EInputType InNewInputType);

private:
	void ShowDebugCollider();
	void ShowDebugGAS();

private:
	// UI
	void CreateOrBindMainHUD();
	void RefreshRaidHUDVisibility();

private:
	bool bShowColliderDebug = false;
	bool bShowGASDebug = false;

private:
	UPROPERTY(VisibleAnywhere, Category = "Input", meta = (AllowPrivateAccess = "true"))
	EInputType CurrentInputType;

	UPROPERTY(EditAnywhere, Category = "Input", meta = (AllowPrivateAccess = "true"))
	TMap<EInputType, TObjectPtr<class UInputMappingContext>> InputMappingContexts;

	UPROPERTY(EditAnywhere, Category = "Input", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> ColliderDebugAction;

	UPROPERTY(EditAnywhere, Category = "Input", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> GASDebugAction;

protected:
	// UI 관련
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = RaidHUD)
	TSubclassOf<UAOMainHUDWidget> MainHUDClass;

	TObjectPtr<UAOMainHUDWidget> MainHUD;
};
