// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/AOClassSwitcherWidget.h"
#include "Components/WidgetSwitcher.h"


void UAOClassSwitcherWidget::SetClassWidget(uint8 ClassType)
{
	UWidget* TargetWidget = nullptr;

	switch (ClassType)
	{
	case 1:
		TargetWidget = AssassinClassWidget;
		break;
	case 2:
		TargetWidget = ClericClassWidget;
		break;
	case 3:
		TargetWidget = RangerClassWidget;
		break;
	case 4:
		TargetWidget = TemplarClassWidget;
		break;
	default:
		TargetWidget = None_Class;
		break;
	}


	if (TargetWidget)
	{
		WidgetSwitcher_Class->SetActiveWidget(TargetWidget);
	}
}
