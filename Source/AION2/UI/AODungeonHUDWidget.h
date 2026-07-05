// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/AOUserWidgetBase.h"
#include "AODungeonHUDWidget.generated.h"

class UAOMonsterHUDWidget;
class AAOMonsterBase;

/**
 * 
 */
UCLASS()
class AION2_API UAODungeonHUDWidget : public UAOUserWidgetBase
{
	GENERATED_BODY()
	
public:
	/*
	* Boss가 Player를 처음 인식했을 때 Full Head-up Display를 보여줄 수 있도록
	* AIMonsterControllerBase->AOMainHUDWidget에서 불러준다.
	*/
	void SetBossHUDVisible(ESlateVisibility InVisibility, AAOMonsterBase* Boss);

	// 보스에게서 Player가 멀어짐
	void HideBossHUDOnly();

	// Boss가 죽음 => 정보 지우기
	void ClearBossHUD();

protected:
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
	TObjectPtr<UAOMonsterHUDWidget> BossHUDWidget;
	
};
