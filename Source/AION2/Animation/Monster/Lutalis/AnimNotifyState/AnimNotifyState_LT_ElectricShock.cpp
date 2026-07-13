// Fill out your copyright notice in the Description page of Project Settings.

#include "Animation/Monster/Lutalis/AnimNotifyState/AnimNotifyState_LT_ElectricShock.h"

#include "Character/Monster/Boss/Lutalis/Lutalis.h"
#include "Components/SkeletalMeshComponent.h"

void UAnimNotifyState_LT_ElectricShock::NotifyBegin(
	USkeletalMeshComponent* MeshComp,
	UAnimSequenceBase* Animation,
	float TotalDuration,
	const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	if (!MeshComp)
	{
		return;
	}

	AActor* OwnerActor = MeshComp->GetOwner();
	if (!OwnerActor || !OwnerActor->HasAuthority())
	{
		return;
	}

	ALutalis* Lutalis = Cast<ALutalis>(OwnerActor);
	if (!Lutalis)
	{
		return;
	}

	Lutalis->BeginElectricShockWarning(TotalDuration);
}

void UAnimNotifyState_LT_ElectricShock::NotifyEnd(
	USkeletalMeshComponent* MeshComp,
	UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);

	if (!MeshComp)
	{
		return;
	}

	AActor* OwnerActor = MeshComp->GetOwner();
	if (!OwnerActor || !OwnerActor->HasAuthority())
	{
		return;
	}

	ALutalis* Lutalis = Cast<ALutalis>(OwnerActor);
	if (!Lutalis)
	{
		return;
	}

	Lutalis->ActivateElectricShockStrikes();
}
