#include "Character/AOCharacter.h"
#include "Player/AOPlayerState.h"

#include "AbilitySystemComponent.h"

AAOCharacter::AAOCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AAOCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

void AAOCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

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

void AAOCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

UAbilitySystemComponent* AAOCharacter::GetAbilitySystemComponent() const
{
	return ASC;
}

void AAOCharacter::InitGAS()
{
	AAOPlayerState* GASPS = GetPlayerState<AAOPlayerState>();
	if (!GASPS)
	{
		return;
	}

	ASC = GASPS->GetAbilitySystemComponent();
	ASC->InitAbilityActorInfo(GASPS, this);
}
