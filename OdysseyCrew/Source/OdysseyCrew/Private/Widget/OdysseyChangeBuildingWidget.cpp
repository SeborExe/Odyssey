// Fill out your copyright notice in the Description page of Project Settings.

#include "Widget/OdysseyChangeBuildingWidget.h"

#include "Components/Button.h"

void UOdysseyChangeBuildingWidget::NativeConstruct()
{
	Super::NativeConstruct();
	if (ChangeBuildingButton && ChangeBuildingButton->OnClicked.IsAlreadyBound(this, &UOdysseyChangeBuildingWidget::NativeConstruct) == false)
	{
		ChangeBuildingButton->OnClicked.AddDynamic(this, &ThisClass::OnButtonClicked_Callback);
	}
}

void UOdysseyChangeBuildingWidget::NativeDestruct()
{
	Super::NativeDestruct();
	if (ChangeBuildingButton->OnClicked.IsAlreadyBound(this, &UOdysseyChangeBuildingWidget::NativeConstruct) == true)
	{
		ChangeBuildingButton->OnClicked.RemoveDynamic(this, &ThisClass::OnButtonClicked_Callback);
	}
}

void UOdysseyChangeBuildingWidget::OnButtonClicked_Callback()
{
	OnChangeBuildingButtonClicked.Broadcast(bNext);
}

void UOdysseyChangeBuildingWidget::ChangeEnableState(bool bEnabled)
{
	if (ChangeBuildingButton)
		ChangeBuildingButton->SetIsEnabled(bEnabled);
}

void UOdysseyChangeBuildingWidget::ChangeVisibility(bool bVisible)
{
	ESlateVisibility EVisibility = bVisible ? ESlateVisibility::Visible : ESlateVisibility::Collapsed;
	SetVisibility(EVisibility);
}
