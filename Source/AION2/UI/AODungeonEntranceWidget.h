// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "AODungeonEntranceWidget.generated.h"

/**
 *
 */
UCLASS()
class AION2_API UAODungeonEntranceWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidget))
	class UButton* EnterButton;

	UPROPERTY(meta = (BindWidget))
	class UButton* CreateButton;

private:
	UFUNCTION()
	void OnEnterButtonClicked();

	UFUNCTION()
	void OnCreateButtonClicked();

};
