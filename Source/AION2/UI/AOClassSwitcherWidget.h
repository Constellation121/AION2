// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/AOUserWidgetBase.h"
#include "AOClassSwitcherWidget.generated.h"

/**
 * 
 */
UCLASS()
class AION2_API UAOClassSwitcherWidget : public UAOUserWidgetBase
{
	GENERATED_BODY()
	
public:
	void SetClassWidget(uint8 ClassType);
	
protected:
	UPROPERTY(meta = (BindWidget))
	class UWidgetSwitcher* WidgetSwitcher_Class;

	UPROPERTY(meta = (BindWidget))
	class UUserWidget* None_Class;

	UPROPERTY(meta = (BindWidget))
	class UUserWidget* AssassinClassWidget;

	UPROPERTY(meta = (BindWidget))
	class UUserWidget* ClericClassWidget;

	UPROPERTY(meta = (BindWidget))
	class UUserWidget* RangerClassWidget;

	UPROPERTY(meta = (BindWidget))
	class UUserWidget* TemplarClassWidget;


};
