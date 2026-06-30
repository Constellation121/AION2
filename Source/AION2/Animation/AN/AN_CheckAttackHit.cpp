#include "Animation/AN/AN_CheckAttackHit.h"
#include "GAS/AOGameplayTags.h"

#include "AbilitySystemBlueprintLibrary.h"

void UAN_CheckAttackHit::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	UE_LOG(
		LogTemp,
		Warning,
		TEXT("[DamageTrace][AnimNotify_CheckAttackHit][Enter] Mesh=%s Owner=%s Animation=%s"),
		*GetNameSafe(MeshComp),
		MeshComp ? *GetNameSafe(MeshComp->GetOwner()) : TEXT("None"),
		*GetNameSafe(Animation)
	);

	if (MeshComp)
	{
		AActor* OwnerActor = MeshComp->GetOwner();
		if (OwnerActor)
		{
			UE_LOG(
				LogTemp,
				Warning,
				TEXT("[DamageTrace][AnimNotify_CheckAttackHit][SendGameplayEvent] Owner=%s NetMode=%d HasAuthority=%d Tag=%s"),
				*GetNameSafe(OwnerActor),
				static_cast<int32>(OwnerActor->GetNetMode()),
				OwnerActor->HasAuthority() ? 1 : 0,
				*EVENT_CHECKATTACKHIT.ToString()
			);

			FGameplayEventData PayloadData;
			UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(OwnerActor, EVENT_CHECKATTACKHIT, PayloadData);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("[DamageTrace][AnimNotify_CheckAttackHit][Abort] Reason=OwnerActorNull Mesh=%s"), *GetNameSafe(MeshComp));
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[DamageTrace][AnimNotify_CheckAttackHit][Abort] Reason=MeshCompNull"));
	}
}
