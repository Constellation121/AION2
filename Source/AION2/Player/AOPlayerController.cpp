#include "Player/AOPlayerController.h"

#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"

#include "UI/AOMainHUDWidget.h"
#include "Components/Widget.h"
#include "Player/AOPlayerState.h"
#include "Game/AORaidGameState.h"

AAOPlayerController::AAOPlayerController()
{
	CurrentInputType = EInputType::Game;
}

void AAOPlayerController::HandlePawnASCReady()
{
	if (GetNetMode() == NM_DedicatedServer || !IsLocalController())
	{
		return;
	}

	CreateOrBindMainHUD();
}

void AAOPlayerController::BeginPlay()
{
	Super::BeginPlay();

	bShowGASDebug = false;

	/*
	* UI 생성/렌더/Slate/Viewport/InputMode 코드는
	* GetNetMode() != NM_DedicatedServer && IsLocalController() 조건 아래에서만 실행
	* => Local Client인 경우에만 UI 생성
	*/
	if (GetNetMode() == NM_DedicatedServer || !IsLocalController())
	{
		return;
	}

	FInputModeGameOnly GameOnlyInputMode;
	SetInputMode(GameOnlyInputMode);

	SetInputMappingContext(CurrentInputType);
}

void AAOPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent);
	if (EnhancedInputComponent)
	{
		EnhancedInputComponent->BindAction(GASDebugAction, ETriggerEvent::Started, this, &AAOPlayerController::ShowDebugGAS);
	}
}

void AAOPlayerController::SetInputMappingContext(EInputType InNewInputType)
{
	// Exception Handling
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

	// 정상인 경우에만 실행
	InputSystem->ClearAllMappings();
	InputSystem->AddMappingContext(InputMappingContexts[InNewInputType], 0);
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
			return;
		}

		MainHUD->AddToViewport();
	}

	AAOPlayerState* AOPlayerState = GetPlayerState<AAOPlayerState>();
	MainHUD->BindToPlayerState(AOPlayerState);

}

void AAOPlayerController::RefreshRaidHUDVisibility()
{
	if (!MainHUD)
	{
		return;
	}

	const bool bIsRaidLevel =
		GetWorld() && GetWorld()->GetGameState<AAORaidGameState>() != nullptr;

	MainHUD->SetRaidHUDVisible(bIsRaidLevel);
}
