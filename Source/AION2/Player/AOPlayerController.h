#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "AOPlayerController.generated.h"

extern TAutoConsoleVariable<int32> CVarDrawAttackTrace;

class UAOMainHUDWidget;
class AAOMonsterBase;

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
	// Unreal Dedicated Server AI 감지 흐름
	UFUNCTION(Client, Reliable)
	void Client_ShowBossHUD(AAOMonsterBase* Boss);

	UFUNCTION(Client, Reliable)
	void Client_HideBossHUDOnly(AAOMonsterBase* Boss);
	void HideBossHUDOnly(AAOMonsterBase* Boss);


protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;
	virtual void AcknowledgePossession(APawn* P) override;

private:
	void SetInputMappingContext(EInputType InNewInputType);


private:
	void ShowDebugCollider();
	void ShowDebugGAS();


public:
	// UI
	void HandlePawnASCReady();

private:
	// UI
	void CreateOrBindMainHUD();

public:
	UAOMainHUDWidget* GetMainHUD() const { return MainHUD; }

public:
	// BossHUD
	void ShowBossHUD(AAOMonsterBase* Boss);
	void HideBossHUDOnly();
	void ClearBossHUD();


	// AIMonsterControllerBase에서 호출
	UFUNCTION(Client, Reliable)
	void Client_ClearBossHUD(AAOMonsterBase* Boss);

	void ClearBossHUD(AAOMonsterBase* Boss);

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

private:
	UPROPERTY()
	TObjectPtr<AAOMonsterBase> CurrentBossHUDTarget;
};
