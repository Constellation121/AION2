#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "AOPlayerController.generated.h"

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
	// PlayerBase(Daeva)가 준비됐음을 알리면(OnRep_PlayerState) 실행될 함수.
	void HandlePawnASCReady();

protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;

private:
	void SetInputMappingContext(EInputType InNewInputType);

private:
	void ShowDebugGAS();
	bool bShowGASDebug = false;

private:
	// UI 관련
	void CreateOrBindMainHUD();
	// RaidLevel에 있으면 (RaidGameState가 있으면) RaidHUD 보이기.
	void RefreshRaidHUDVisibility();

private:
	UPROPERTY(VisibleAnywhere, Category = "Input", meta = (AllowPrivateAccess = "true"))
	EInputType CurrentInputType;

	UPROPERTY(EditAnywhere, Category = "Input", meta = (AllowPrivateAccess = "true"))
	TMap<EInputType, TObjectPtr<class UInputMappingContext>> InputMappingContexts;

	UPROPERTY(EditAnywhere, Category = "Input", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> GASDebugAction;


protected:
	// UI 관련
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = RaidHUD)
	TSubclassOf<UAOMainHUDWidget> MainHUDClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = RaidHUD)
	TObjectPtr<UAOMainHUDWidget> MainHUD;
};
