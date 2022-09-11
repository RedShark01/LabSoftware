// Fill out your copyright notice in the Description page of Project Settings.


#include "HealthBarComponent.h"
#include "UObject/ConstructorHelpers.h"

UHealthBarComponent::UHealthBarComponent()
{
	static ConstructorHelpers::FClassFinder<UUserWidget> UserWidget(TEXT("/Game/Widgets/W_HealthBar"));

	if (UserWidget.Class != NULL)
		WidgetClass = UserWidget.Class;

	Space = EWidgetSpace::Screen;

	DrawSize = { 100, 10 };

}