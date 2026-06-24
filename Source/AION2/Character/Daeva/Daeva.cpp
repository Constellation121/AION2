#include "Character/Daeva/Daeva.h"
#include "Player/AOPlayerState.h"
#include "GAS/AOGameplayTags.h"
#include "Character/AOCharacterMovementComponent.h"
#include "Data/DA_AbilitySet.h"

#include "GameplayTagContainer.h"
#include "AbilitySystemComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Camera/CameraComponent.h"
#include "EnhancedInputComponent.h"
#include "GAS/AttributeSet/AOAttributeSet.h"

ADaeva::ADaeva(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 720.0f, 0.0f);
	GetCharacterMovement()->MaxWalkSpeed = 600.0f;
	GetCharacterMovement()->JumpZVelocity = 520.0f;

	GetMesh()->SetRelativeLocationAndRotation(FVector(0.0f, 0.0f, -90.0f), FRotator(0.0f, -90.0f, 0.0f));
	GetMesh()->SetVisibility(false);
	GetMesh()->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::AlwaysTickPoseAndRefreshBones;

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->TargetArmLength = 500.0f;
	SpringArm->SetRelativeLocation(FVector(0.0f, 0.0f, 75.0f));
	SpringArm->bUsePawnControlRotation = true;
	SpringArm->bEnableCameraLag = true;
	SpringArm->bEnableCameraRotationLag = true;

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm);

	CreatePart(EDaevaPartType::Helmet, TEXT("HelmetPart"));
	CreatePart(EDaevaPartType::Hair, TEXT("HairPart"));
	CreatePart(EDaevaPartType::Head, TEXT("HeadPart"));
	CreatePart(EDaevaPartType::Shoulder, TEXT("ShoulderPart"));
	CreatePart(EDaevaPartType::Cape, TEXT("CapePart"));
	CreatePart(EDaevaPartType::Body, TEXT("BodyPart"));
	CreatePart(EDaevaPartType::Glove, TEXT("GlovePart"));
	CreatePart(EDaevaPartType::Pants, TEXT("PantsPart"));
	CreatePart(EDaevaPartType::Boots, TEXT("BootsPart"));

	Weapon = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Weapon"));
	Weapon->SetVisibility(false);

	SubWeapon = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SubWeapon"));
	SubWeapon->SetVisibility(false);

	Wing = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Wing"));
	Wing->SetupAttachment(GetMesh(), TEXT("Wing_Root"));
	Wing->SetVisibility(false);
}

void ADaeva::Multicast_PlayWingMontage_Implementation(EMontageID MontageID, float PlayRate)
{
	if (!Wing || !WingMontages[MontageID])
	{
		return;
	}

	if (UAnimInstance* WingAnimInstance = Wing->GetAnimInstance())
	{
		WingAnimInstance->Montage_Play(WingMontages[MontageID], PlayRate);
	}
}

void ADaeva::Multicast_SetWingVisibility_Implementation(bool NewVisible)
{
	SetWingVisibility(NewVisible);
}

void ADaeva::BeginPlay()
{
	Super::BeginPlay();

	TargetZoomDistance = SpringArm->TargetArmLength;
}

void ADaeva::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	Tick_Camera(DeltaTime);
}

void ADaeva::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	InitGAS();
}

void ADaeva::UnPossessed()
{
	ClearGAS();

	Super::UnPossessed();
}

void ADaeva::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	InitGAS();
}

void ADaeva::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	if (EnhancedInputComponent)
	{
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ADaeva::Move);
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ADaeva::Look);
		EnhancedInputComponent->BindAction(ZoomAction, ETriggerEvent::Triggered, this, &ADaeva::Zoom);
		EnhancedInputComponent->BindAction(ShiftAction, ETriggerEvent::Started, this, &ADaeva::InputShiftPressed);
		EnhancedInputComponent->BindAction(SpaceAction, ETriggerEvent::Started, this, &ADaeva::InputSpacePressed);
		EnhancedInputComponent->BindAction(LBAction, ETriggerEvent::Triggered, this, &ADaeva::InputLBPressed);

		/*EnhancedInputComponent->BindAction(
			ShiftAction,
			ETriggerEvent::Completed,
			this,
			&ADaeva::InputShiftReleased
		);*/

		EnhancedInputComponent->BindAction(
			MoveAction,
			ETriggerEvent::Completed,
			this,
			&ADaeva::InputMoveReleased
		);
	}


}

void ADaeva::Tick_Camera(float DeltaTime)
{
	SpringArm->TargetArmLength = FMath::FInterpTo(SpringArm->TargetArmLength, TargetZoomDistance, DeltaTime, 10.f);
}

void ADaeva::Move(const FInputActionValue& Value)
{
	FVector2D Movement = Value.Get<FVector2D>();

	FRotator Rotation = GetControlRotation();
	FRotator YawRotation(0.0f, Rotation.Yaw, 0.0f);

	FVector ForwardVector = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	FVector RightVector = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	FVector NewMoveInputDirection =
		ForwardVector * Movement.Y +
		RightVector * Movement.X;

	if (NewMoveInputDirection.IsNearlyZero())
	{
		return;
	}

	bHasMoveInput = true;

	CurrentMoveInputDirection = NewMoveInputDirection.GetSafeNormal();
	AddMovementInput(CurrentMoveInputDirection);

	
	/*if (bSprintInputHeld)
	{
		RequestStartSprint();
	}*/
}

void ADaeva::Look(const FInputActionValue& Value)
{
	FVector2D RotationValue = Value.Get<FVector2D>();

	AddControllerYawInput(RotationValue.X);
	AddControllerPitchInput(-RotationValue.Y);
}

void ADaeva::Zoom(const FInputActionValue& Value)
{
	const float AxisValue = Value.Get<float>();

	TargetZoomDistance -= AxisValue * ZoomSpeed;
	TargetZoomDistance = FMath::Clamp(TargetZoomDistance, MinZoomDistance, MaxZoomDistance);
}

void ADaeva::InitGAS()
{
	AAOPlayerState* GASPS = GetPlayerState<AAOPlayerState>();
	if (!GASPS)
	{
		return;
	}

	ASC = GASPS->GetAbilitySystemComponent();
	ASC->InitAbilityActorInfo(GASPS, this);

	if (!SprintStaminaChangedDelegateHandle.IsValid())
	{
		SprintStaminaChangedDelegateHandle =
			ASC->GetGameplayAttributeValueChangeDelegate(
				UAOAttributeSet::GetStaminaAttribute()
			).AddUObject(this, &ADaeva::OnStaminaChangedForSprint);
	}

	BindMoveSpeedAttribute();

	if (!bTagEventsRegistered)
	{
		ASC->RegisterGameplayTagEvent(STATE_COMBAT, EGameplayTagEventType::NewOrRemoved).AddUObject(this, &ADaeva::OnCombatStateChanged);

		bTagEventsRegistered = true;
	}

	if (HasAuthority())
	{
		GASPS->GiveCommonAbilities();
		CombatAbilitySet->GiveToASC(ASC, CombatAbilityHandles);
		ApplyDashStaminaRegenEffect();
	}
}

void ADaeva::ClearGAS()
{
	if (ASC && bMoveSpeedDelegateRegistered)
	{
		ASC->GetGameplayAttributeValueChangeDelegate(
			UAOAttributeSet::GetMoveSpeedAttribute()
		).Remove(MoveSpeedChangedDelegateHandle);

		MoveSpeedChangedDelegateHandle.Reset();
		bMoveSpeedDelegateRegistered = false;
	}

	if (HasAuthority())
	{
		for (FGameplayAbilitySpecHandle Handle : CombatAbilityHandles)
		{
			ASC->ClearAbility(Handle);
		}

		CombatAbilityHandles.Empty();
	}
}

void ADaeva::GASInputPressed(int32 InputId)
{
	if (FGameplayAbilitySpec* Spec = ASC->FindAbilitySpecFromInputID(InputId))
	{
		Spec->InputPressed = true;
		if (Spec->IsActive())
		{
			ASC->AbilitySpecInputPressed(*Spec);
		}
		else
		{
			ASC->TryActivateAbility(Spec->Handle);
		}
	}
}

void ADaeva::GASInputReleased(int32 InputId)
{
	if (FGameplayAbilitySpec* Spec = ASC->FindAbilitySpecFromInputID(InputId))
	{
		Spec->InputPressed = false;
		if (Spec->IsActive())
		{
			ASC->AbilitySpecInputReleased(*Spec);
		}
	}
}

void ADaeva::ApplyDashStaminaRegenEffect()
{
	if (!HasAuthority())
	{
		return;
	}

	if (!ASC || !DashStaminaRegenEffect)
	{
		return;
	}

	FGameplayEffectContextHandle EffectContext = ASC->MakeEffectContext();
	EffectContext.AddSourceObject(this);

	FGameplayEffectSpecHandle SpecHandle =
		ASC->MakeOutgoingSpec(DashStaminaRegenEffect, 1.0f, EffectContext);

	if (SpecHandle.IsValid())
	{
		ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
	}
}

void ADaeva::BindMoveSpeedAttribute()
{
	if (!ASC || bMoveSpeedDelegateRegistered)
	{
		return;
	}

	MoveSpeedChangedDelegateHandle =
		ASC->GetGameplayAttributeValueChangeDelegate(
			UAOAttributeSet::GetMoveSpeedAttribute()
		).AddUObject(this, &ADaeva::OnMoveSpeedChanged);

	bMoveSpeedDelegateRegistered = true;

	const float CurrentMoveSpeed =
		ASC->GetNumericAttribute(UAOAttributeSet::GetMoveSpeedAttribute());

	GetCharacterMovement()->MaxWalkSpeed = CurrentMoveSpeed;

	UE_LOG(LogTemp, Log, TEXT("[MoveSpeed] Initial Apply: %.1f"), CurrentMoveSpeed);
}

void ADaeva::OnMoveSpeedChanged(const FOnAttributeChangeData& Data)
{
	if (!GetCharacterMovement())
	{
		return;
	}

	GetCharacterMovement()->MaxWalkSpeed = Data.NewValue;

	UE_LOG(
		LogTemp,
		Log,
		TEXT("[MoveSpeed] %.1f -> %.1f"),
		Data.OldValue,
		Data.NewValue
	);
}

void ADaeva::InputShiftPressed()
{
	//bSprintInputHeld = true;

	if (IsSprinting())
	{
		RequestStopSprint();
		return;
	}

	GASInputPressed(static_cast<int32>(EAbilityID::Dash));

	if (bHasMoveInput)
	{
		RequestStartSprint();
	}
}

void ADaeva::InputSpacePressed()
{
	if (GetCharacterMovement()->MovementMode == MOVE_Custom &&
		GetCharacterMovement()->CustomMovementMode == static_cast<uint8>(EAOMovementMode::Glide))
	{
		GASInputPressed(static_cast<int32>(EAbilityID::StopGlide));
		return;
	}

	if (GetCharacterMovement()->IsFalling())
	{
		GASInputPressed(static_cast<int32>(EAbilityID::Glide));
		return;
	}

	GASInputPressed(static_cast<int32>(EAbilityID::Jump));
}

void ADaeva::InputLBPressed()
{
	GASInputReleased(static_cast<int32>(EAbilityID::Dash));

	if (ASC->HasMatchingGameplayTag(COMBO_AVAILABLE_LB2))
	{
		GASInputPressed(static_cast<int32>(EAbilityID::LB_2));
	}
	else if (ASC->HasMatchingGameplayTag(COMBO_AVAILABLE_LB3))
	{
		GASInputPressed(static_cast<int32>(EAbilityID::LB_3));
	}
	else
	{
		GASInputPressed(static_cast<int32>(EAbilityID::LB_1));
	}
}

void ADaeva::InputMoveReleased()
{
	bHasMoveInput = false;
	RequestStopSprint();
}

void ADaeva::OnCombatStateChanged(const FGameplayTag Tag, int32 NewCount)
{
	const bool bIsCombat = NewCount > 0;

	SetWeaponVisibility(bIsCombat);
	SetSubWeaponVisibility(bIsCombat);
}

void ADaeva::StartSprint()
{
	if (!ASC || !HasAuthority())
	{
		return;
	}

	if (SprintEffectHandle.IsValid())
	{
		return;
	}

	const UAOAttributeSet* AttributeSet = ASC->GetSet<UAOAttributeSet>();
	if (!AttributeSet || AttributeSet->GetStamina() <= 0.0f)
	{
		return;
	}

	if (!SprintEffect || !SprintDrainEffect)
	{
		UE_LOG(LogTemp, Error, TEXT("[Sprint] Sprint GE reference missing"));
		return;
	}

	FGameplayEffectContextHandle SprintContext = ASC->MakeEffectContext();
	SprintContext.AddSourceObject(this);

	FGameplayEffectSpecHandle SprintSpec =
		ASC->MakeOutgoingSpec(SprintEffect, 1.0f, SprintContext);

	if (!SprintSpec.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("[Sprint] SprintSpec invalid"));
		return;
	}

	SprintEffectHandle =
		ASC->ApplyGameplayEffectSpecToSelf(*SprintSpec.Data.Get());

	UE_LOG(
		LogTemp,
		Warning,
		TEXT("[Sprint] Sprint applied / HandleValid=%d / Speed=%.1f"),
		SprintEffectHandle.IsValid(),
		ASC->GetNumericAttribute(UAOAttributeSet::GetMoveSpeedAttribute())
	);

	FGameplayEffectContextHandle DrainContext = ASC->MakeEffectContext();
	DrainContext.AddSourceObject(this);

	FGameplayEffectSpecHandle DrainSpec =
		ASC->MakeOutgoingSpec(SprintDrainEffect, 1.0f, DrainContext);

	if (!DrainSpec.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("[Sprint] DrainSpec invalid"));
		return;
	}

	SprintDrainEffectHandle =
		ASC->ApplyGameplayEffectSpecToSelf(*DrainSpec.Data.Get());

	UE_LOG(
		LogTemp,
		Warning,
		TEXT("[Sprint] Drain applied / HandleValid=%d"),
		SprintDrainEffectHandle.IsValid()
	);
}

void ADaeva::StopSprint()
{
	if (!ASC || !HasAuthority())
	{
		return;
	}

	if (SprintEffectHandle.IsValid())
	{
		ASC->RemoveActiveGameplayEffect(SprintEffectHandle);
		SprintEffectHandle.Invalidate();
	}

	if (SprintDrainEffectHandle.IsValid())
	{
		ASC->RemoveActiveGameplayEffect(SprintDrainEffectHandle);
		SprintDrainEffectHandle.Invalidate();
	}

	UE_LOG(LogTemp, Warning, TEXT("[Sprint] Stopped"));
}

bool ADaeva::IsSprinting() const
{
	if (!ASC)
	{
		return false;
	}

	const FGameplayTag SprintTag =
		FGameplayTag::RequestGameplayTag(FName("State.Sprint"));

	return ASC->HasMatchingGameplayTag(SprintTag);
}

void ADaeva::OnStaminaChangedForSprint(const FOnAttributeChangeData& Data)
{
	if (Data.NewValue > 0.0f)
	{
		return;
	}

	StopSprint();
}

void ADaeva::InputShiftReleased()
{
	//bSprintInputHeld = false;
	RequestStopSprint();
}

void ADaeva::ServerStartSprint_Implementation()
{
	StartSprint();
}

void ADaeva::ServerStopSprint_Implementation()
{
	StopSprint();
}

void ADaeva::RequestStartSprint()
{
	if (HasAuthority())
	{
		StartSprint();
	}
	else
	{
		ServerStartSprint();
	}
}

void ADaeva::RequestStopSprint()
{
	if (HasAuthority())
	{
		StopSprint();
	}
	else
	{
		ServerStopSprint();
	}
}

void ADaeva::SetWeaponVisibility(bool NewVisible)
{
	if (Weapon)
	{
		Weapon->SetVisibility(NewVisible);
	}
}

void ADaeva::SetSubWeaponVisibility(bool NewVisible)
{
	if (SubWeapon)
	{
		SubWeapon->SetVisibility(NewVisible);
	}
}

void ADaeva::SetWingVisibility(bool NewVisible)
{
	if (Wing)
	{
		Wing->SetVisibility(NewVisible);
	}
}

void ADaeva::CreatePart(EDaevaPartType PartType, const TCHAR* ComponentName)
{
	USkeletalMeshComponent* PartMesh = CreateDefaultSubobject<USkeletalMeshComponent>(ComponentName);

	PartMesh->SetupAttachment(GetMesh());
	PartMesh->SetLeaderPoseComponent(GetMesh());

	Parts.Add(PartType, PartMesh);
}
