#include "Player/AOPlayerState.h"
#include "AbilitySystemComponent.h"

AAOPlayerState::AAOPlayerState()
{
    NetUpdateFrequency = 100.f;

	ASC = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("ASC"));
	ASC->SetIsReplicated(true);
}

UAbilitySystemComponent* AAOPlayerState::GetAbilitySystemComponent() const
{
	return ASC;
}
