#include "AI/Siliator/SiliatorAIController.h"
#include "Character/Monster/AOMonsterBase.h"
#include "GAS/AOGameplayTags.h"

#include "Perception/AIPerceptionTypes.h"

void ASiliatorAIController::TargetPerceptionOn(AActor* Actor, FAIStimulus Stimlus)
{
	// 수연 super를 상속 받아서 맨 앞에 넣어줘야 Boss HUD가 보임
	Super::TargetPerceptionOn(Actor, Stimlus);

	// 감지 여부 저장/타겟 배열/HUD/현재 타겟은 base가 함, 자식은 페이즈만 COMBAT으로 변경
	if (HasAuthority() == false || !Stimlus.WasSuccessfullySensed())
	{
		return;
	}

	if (ControlledMonster)
	{
		PhaseTag = PHASE_MONSTER_COMBAT;
		ControlledMonster->Set_Phase(PHASE_MONSTER_COMBAT);
	}

	// 기존 코드: 감지 여부 저장, 타겟 배열 추가, 현재 타겟 설정, 페이즈 변경을 전부 자식이 함
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
