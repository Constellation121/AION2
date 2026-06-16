#include "Character/AOCharacter.h"
#include "Character/AOCharacterMovementComponent.h"

#include "Player/AOPlayerState.h"
#include "GAS/AttributeSet/AOAttributeSet.h"

#include "AbilitySystemComponent.h"
#include "GameplayEffect.h"
#include "UObject/ConstructorHelpers.h"

AAOCharacter::AAOCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UAOCharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
{
	PrimaryActorTick.bCanEverTick = true;


	static ConstructorHelpers::FClassFinder<UGameplayEffect> DashStaminaRegenGEClass(
		TEXT("/Game/Blueprint/GAS/GE/Cost/GE_Regen_DashStamina")
	);

	if (DashStaminaRegenGEClass.Succeeded())
	{
		DashStaminaRegenEffect = DashStaminaRegenGEClass.Class;
		UE_LOG(LogTemp, Warning, TEXT("[GAS] DashStaminaRegenEffect loaded successfully."));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("[GAS] Failed to load GE_Regen_DashStamina."));
	}
}

UAbilitySystemComponent* AAOCharacter::GetAbilitySystemComponent() const
{
	return ASC;
}

void AAOCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	InitGAS();
}

void AAOCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	InitGAS();
}

void AAOCharacter::InitGAS()
{
	// PlayerState -> ASC
	AAOPlayerState* AOPlayerState = GetPlayerState<AAOPlayerState>();

	if (!AOPlayerState)
	{
		return;
	}

	ASC = AOPlayerState->GetAbilitySystemComponent();
	AttributeSet = AOPlayerState->GetAttributeSet();

	if (!ASC)
	{
		return;
	}

	ASC->InitAbilityActorInfo(AOPlayerState, this);

	if (HasAuthority())
	{
		AOPlayerState->GiveCommonAbilities();
		InitializeDefaultAttributes();
	}


	ApplyDashStaminaRegenEffect();

}

void AAOCharacter::InitializeDefaultAttributes()
{
	if (!ASC || !DefaultAttributeEffect)
	{
		return;
	}

	FGameplayEffectContextHandle ContextHandle = ASC->MakeEffectContext();
	ContextHandle.AddSourceObject(this);

	FGameplayEffectSpecHandle SpecHandle =
		ASC->MakeOutgoingSpec(DefaultAttributeEffect, 1.f, ContextHandle);

	if (SpecHandle.IsValid())
	{
		ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
	}
}

void AAOCharacter::ApplyDashStaminaRegenEffect()
{
	if (!ASC)
	{
		UE_LOG(LogTemp, Error, TEXT("[GAS] ASC is null. Cannot apply stamina regen."));
		return;
	}

	if (!DashStaminaRegenEffect)
	{
		UE_LOG(LogTemp, Error, TEXT("[GAS] DashStaminaRegenEffect is null."));
		return;
	}

	FGameplayEffectContextHandle ContextHandle = ASC->MakeEffectContext();
	ContextHandle.AddSourceObject(this);

	FGameplayEffectSpecHandle SpecHandle =
		ASC->MakeOutgoingSpec(DashStaminaRegenEffect, 1.f, ContextHandle);

	if (SpecHandle.IsValid())
	{
		ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());

		UE_LOG(LogTemp, Warning, TEXT("[GAS] Dash stamina regen effect applied."));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("[GAS] Failed to make stamina regen spec."));
	}
}