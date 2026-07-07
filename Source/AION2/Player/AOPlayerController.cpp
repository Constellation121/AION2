#include "Player/AOPlayerController.h"

#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"

#include "UI/AOMainHUDWidget.h"
#include "Components/Widget.h"
#include "Player/AOPlayerState.h"
#include "Character/Monster/AOMonsterBase.h"
#include "Character/Daeva/Daeva.h"


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
	/* Suyeon: More strict validation Check => else: retry next Tick(26.07.07) */

	// Exception Handling 
	// => Validation Check: Is LocalPlayer && DedicatedServer => Can Show UI
	// Existed Validation Check
	if (GetNetMode() == NM_DedicatedServer || !IsLocalController())
	{
		return;
	}

	// === Validation Check below are the new ones ===

	// => Validation Check: Is the local pawn is daeva?
	ADaeva* Daeva = Cast<ADaeva>(GetPawn());
	if (!Daeva)
	{
		return;
	}

	// => Validation Check: The Daeva has ASC?
	AAOPlayerState* AOPlayerState = Daeva->GetPlayerState<AAOPlayerState>();
	UAbilitySystemComponent* ASC = Daeva->GetAbilitySystemComponent();


	// => Validation Check: The ASC of Dave is Ready?
	if (!AOPlayerState || !ASC)
	{
		GetWorldTimerManager().SetTimerForNextTick(
			this,
			&AAOPlayerController::HandlePawnASCReady
		);
		return;
	}

	CreateOrBindMainHUD(AOPlayerState);
	Daeva->BindOverheadStatusWidget();
}

void AAOPlayerController::CreateOrBindMainHUD(AAOPlayerState* AOPlayerState)
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

void AAOPlayerController::ShowTargetMonsterHUD(AAOMonsterBase* InMonster)
{
	if (!IsLocalController() || !MainHUD || !InMonster)
	{
		return;
	}

	MainHUD->ShowTargetMonsterHUD(InMonster);
}

void AAOPlayerController::HideTargetMonsterHUD()
{
	if (!IsLocalController() || !MainHUD)
	{
		return;
	}

	MainHUD->HideTargetMonsterHUD();
}

