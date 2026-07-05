#include "Player/AOPlayerController.h"

#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"

#include "UI/AOMainHUDWidget.h"
#include "Components/Widget.h"
#include "Player/AOPlayerState.h"
#include "Character/Monster/AOMonsterBase.h"


TAutoConsoleVariable<int32> CVarDrawAttackTrace(TEXT("ao.Debug.DrawAttackTrace"), 0, TEXT("Draw attack trace debug"), ECVF_Cheat);

AAOPlayerController::AAOPlayerController()
{
	CurrentInputType = EInputType::Game;
}

void AAOPlayerController::Server_SetShowColliderDebug_Implementation()
{
	bShowColliderDebug = !bShowColliderDebug;

	if (bShowColliderDebug)
	{
		ConsoleCommand(TEXT("ao.Debug.DrawAttackTrace 1"));
	}
	else
	{
		ConsoleCommand(TEXT("ao.Debug.DrawAttackTrace 0"));
	}
}

void AAOPlayerController::ShowBossHUD(AAOMonsterBase* Boss)
{
	if (!IsLocalController() || !MainHUD || !Boss)
	{
		return;
	}

	CurrentBossHUDTarget = Boss;
	MainHUD->SetBossHUDVisible(Boss);
}

void AAOPlayerController::Client_ShowBossHUD_Implementation(AAOMonsterBase* Boss)
{
	if (!IsLocalController() || !MainHUD || !Boss)
	{
		UE_LOG(LogTemp, Warning, TEXT("AAOPlayerController::Clinet ShowBossHUD: None, Line %d"), 47);
		return;
	}

	ShowBossHUD(Boss);
}

void AAOPlayerController::Client_HideBossHUDOnly_Implementation(AAOMonsterBase* Boss)
{
	HideBossHUDOnly(Boss);
}

void AAOPlayerController::HideBossHUDOnly(AAOMonsterBase* Boss)
{
	if (!IsLocalController() || !MainHUD || !Boss)
	{
		return;
	}

	if (CurrentBossHUDTarget != Boss)
	{
		return;
	}

	MainHUD->HideBossHUDOnly();
}

void AAOPlayerController::BeginPlay()
{
	Super::BeginPlay();

	// 클라이언트일 때만 지정
	if (GetNetMode() == NM_DedicatedServer || !IsLocalController())
	{
		return;
	}

	FInputModeGameOnly GameOnlyInputMode;
	SetInputMode(GameOnlyInputMode);

	SetInputMappingContext(CurrentInputType);

	bShowGASDebug = false;
}

void AAOPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent);
	if (EnhancedInputComponent)
	{
		EnhancedInputComponent->BindAction(ColliderDebugAction, ETriggerEvent::Started, this, &AAOPlayerController::ShowDebugCollider);
		EnhancedInputComponent->BindAction(GASDebugAction, ETriggerEvent::Started, this, &AAOPlayerController::ShowDebugGAS);
	}
}

// AOPlayerController.cpp
void AAOPlayerController::AcknowledgePossession(APawn* P)
{
	Super::AcknowledgePossession(P);

	if (IsLocalController())
	{
		HandlePawnASCReady();
	}
}

void AAOPlayerController::SetInputMappingContext(EInputType InNewInputType)
{
	ULocalPlayer* LocalPlayer = GetLocalPlayer();
	if (!LocalPlayer)
	{
		return;
	}

	UEnhancedInputLocalPlayerSubsystem* InputSystem =
		ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(LocalPlayer);

	if (!InputSystem)
	{
		return;
	}

	if (!InputMappingContexts.Contains(InNewInputType))
	{
		return;
	}

	InputSystem->ClearAllMappings();
	InputSystem->AddMappingContext(InputMappingContexts[InNewInputType], 0);
}


void AAOPlayerController::ShowDebugCollider()
{
	Server_SetShowColliderDebug();
}

void AAOPlayerController::ShowDebugGAS()
{
	bShowGASDebug = !bShowGASDebug;

	if (bShowGASDebug)
	{
		ConsoleCommand(TEXT("showdebug abilitysystem"));
	}
	else
	{
		ConsoleCommand(TEXT("showdebug none"));
	}
}

void AAOPlayerController::HandlePawnASCReady()
{
	if (GetNetMode() == NM_DedicatedServer || !IsLocalController())
	{
		return;
	}

	CreateOrBindMainHUD();
}

void AAOPlayerController::CreateOrBindMainHUD()
{

	// Exception Handling
	if (GetNetMode() == NM_DedicatedServer || !IsLocalController())
	{
		return;
	}

	if (!MainHUDClass)
	{
		return;
	}

	if (!MainHUD)
	{
		MainHUD = CreateWidget<UAOMainHUDWidget>(this, MainHUDClass);
		if (!MainHUD)
		{
			UE_LOG(LogTemp, Error, TEXT("PlayerContoroller Is Not Vaild"));
			return;
		}

		MainHUD->AddToViewport();
	}

	AAOPlayerState* AOPlayerState = GetPlayerState<AAOPlayerState>();
	MainHUD->BindToPlayerState(AOPlayerState);
}

void AAOPlayerController::Client_ClearBossHUD_Implementation(AAOMonsterBase* Boss)
{
	ClearBossHUD(Boss);
}

void AAOPlayerController::ClearBossHUD(AAOMonsterBase* Boss)
{
	if (!IsLocalController() || !MainHUD || !Boss)
	{
		return;
	}

	if (CurrentBossHUDTarget != Boss)
	{
		return;
	}

	CurrentBossHUDTarget = nullptr;
	MainHUD->ClearBossHUD();
}

