// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Lutalis/AILutalisAIController.h"
#include "Character/Monster/Boss/Lutalis/Lutalis.h"
#include "GAS/AttributeSet/AOAttributeSet.h"
#include "GAS/AOGameplayTags.h"
#include "Perception/AIPerceptionTypes.h"

AAILutalisAIController::AAILutalisAIController()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AAILutalisAIController::TargetPerceptionOn(AActor* Actor, FAIStimulus Stimlus)
{
	// 수연 super를 상속 받아서 맨 앞에 넣어줘야 Boss HUD가 보임
	Super::TargetPerceptionOn(Actor, Stimlus);

	if (HasAuthority() == false || !Stimlus.WasSuccessfullySensed())
	{
		return;
	}

	if (ControlledMonster)
	{
		PhaseTag = PHASE_MONSTER_COMBAT;
		ControlledMonster->Set_Phase(PHASE_MONSTER_COMBAT);
	}

	//if (HasAuthority())
	//{
	//	if (HasDetectedTarget == false)
	//	{
	//		HasDetectedTarget = true;
	//
	//		PhaseTag = PHASE_MONSTER_COMBAT;
	//		ControlledMonster->Set_Phase//(PHASE_MONSTER_COMBAT);
	//
	//		ArrayTargetPlayers.Add(Actor);
	//		CurrentTargetPlayer = Actor;
	//	}
	//}
}


void AAILutalisAIController::BeginPlay()
{
	Super::BeginPlay();


	if (HasAuthority() == false)
	{
		return;
	}

	ControlledLutails = Cast<ALutalis>(ControlledMonster);
	if (IsValid(ControlledLutails))
	{
		ControlledMonsterAttributeSet = ControlledLutails->GetAttributeSet();
	}
}

void AAILutalisAIController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (HasAuthority() == false)
		return;


	if (IsValid(ControlledMonsterAttributeSet) && ControlledMonsterAttributeSet->GetHealth() <= 0)
	{
		PhaseTag = PHASE_MONSTER_DEAD;
	}
}


//void AAILutalisAIController::OnPossess(APawn* InPawn)
//{
//	Super::OnPossess(InPawn);
//
//	if (HasAuthority())
//	{
//		ControlledLutails = Cast<ALutalis>(InPawn);
//	}
//}

