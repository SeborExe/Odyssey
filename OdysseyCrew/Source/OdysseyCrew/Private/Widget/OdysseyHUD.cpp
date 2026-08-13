// Fill out your copyright notice in the Description page of Project Settings.

#include "Widget/OdysseyHUD.h"
#include "Widget/OdysseyMainUserWidget.h"
#include "Blueprint/UserWidget.h"
#include "Core/OdysseyPlayerController.h"

void AOdysseyHUD::BeginPlay()
{
	Super::BeginPlay();
	
	if (OdysseyMainWidgetClass)
	{
		OdysseyMainUserWidget = CreateWidget<UOdysseyMainUserWidget>(GetOwningPlayerController(), OdysseyMainWidgetClass);
		if (OdysseyMainUserWidget)
		{
			OdysseyMainUserWidget->AddToViewport();
			
			if (AOdysseyPlayerController* PC = Cast<AOdysseyPlayerController>(GetOwningPlayerController()))
				PC->BindMainWidget();
		}
	}
}
