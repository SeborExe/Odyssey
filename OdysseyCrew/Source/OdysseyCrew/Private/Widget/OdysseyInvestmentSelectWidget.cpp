// Fill out your copyright notice in the Description page of Project Settings.

#include "Widget/OdysseyInvestmentSelectWidget.h"

#include "Components/PanelWidget.h"
#include "Subsystem/OdysseyDatabaseSubsystem.h"
#include "Widget/OdysseyInvestmentEntryWidget.h"

void UOdysseyInvestmentSelectWidget::Refresh()
{
	if (!ListContainer || !EntryWidgetClass)
	{
		return;
	}
 
	ListContainer->ClearChildren();
 
	UGameInstance* GI = GetGameInstance();
	UOdysseyDatabaseSubsystem* DB = GI ? GI->GetSubsystem<UOdysseyDatabaseSubsystem>() : nullptr;
	if (!DB)
	{
		return;
	}
 
	DB->EnsureOpen();
 
	for (const FOdysseyInvestment& Inv : DB->GetInvestments())
	{
		UOdysseyInvestmentEntryWidget* Entry = CreateWidget<UOdysseyInvestmentEntryWidget>(this, EntryWidgetClass);
		if (!Entry)
		{
			continue;
		}
 
		Entry->SetData(Inv);
		Entry->OnChosen.AddDynamic(this, &UOdysseyInvestmentSelectWidget::HandleEntryChosen);
		ListContainer->AddChild(Entry);
	}
}

void UOdysseyInvestmentSelectWidget::Show() { SetVisibility(ESlateVisibility::Visible); }
void UOdysseyInvestmentSelectWidget::Hide() { SetVisibility(ESlateVisibility::Collapsed); }

void UOdysseyInvestmentSelectWidget::NativeConstruct()
{
	Super::NativeConstruct();
	Refresh();
}

void UOdysseyInvestmentSelectWidget::HandleEntryChosen(int32 InvestmentId)
{
	OnInvestmentChosen.Broadcast(InvestmentId);
}
