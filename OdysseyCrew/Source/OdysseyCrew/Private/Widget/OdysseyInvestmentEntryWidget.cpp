// Fill out your copyright notice in the Description page of Project Settings.

#include "Widget/OdysseyInvestmentEntryWidget.h"

#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Type/OdysseyTypes.h"

void UOdysseyInvestmentEntryWidget::SetData(const FOdysseyInvestment& Investment)
{
	InvestmentId = Investment.Id;
	if (NameText) { NameText->SetText(FText::FromString(Investment.Name.ToString())); }
	if (CityText) { CityText->SetText(FText::FromString(Investment.City.ToString())); }
}

void UOdysseyInvestmentEntryWidget::NativeConstruct()
{
	Super::NativeConstruct();
	if (SelectButton && !SelectButton->OnClicked.IsAlreadyBound(this, &UOdysseyInvestmentEntryWidget::HandleClicked))
	{
		SelectButton->OnClicked.AddDynamic(this, &UOdysseyInvestmentEntryWidget::HandleClicked);
	}
}

void UOdysseyInvestmentEntryWidget::HandleClicked()
{
	OnChosen.Broadcast(InvestmentId);
}
