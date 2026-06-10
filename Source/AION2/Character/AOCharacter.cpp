#include "Character/AOCharacter.h"

AAOCharacter::AAOCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
}

UAbilitySystemComponent* AAOCharacter::GetAbilitySystemComponent() const
{
	return ASC;
}

void AAOCharacter::InitGAS()
{
}
