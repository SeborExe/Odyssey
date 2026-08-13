// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "OdysseyInvestmentEntryWidget.generated.h"

struct FOdysseyInvestment;
class UButton;
class UTextBlock;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInvestmentEntryChosen, int32, InvestmentId);

UCLASS()
class ODYSSEYCREW_API UOdysseyInvestmentEntryWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintAssignable, Category = "Odyssey")
	FOnInvestmentEntryChosen OnChosen;
 
	UFUNCTION(BlueprintCallable, Category = "Odyssey")
	void SetData(const FOdysseyInvestment& Investment);
 
protected:
	virtual void NativeConstruct() override;
	
	UPROPERTY(meta = (BindWidget))         
	UButton* SelectButton = nullptr;
	UPROPERTY(meta = (BindWidgetOptional)) 
	UTextBlock* NameText = nullptr;
	UPROPERTY(meta = (BindWidgetOptional)) 
	UTextBlock* CityText = nullptr;
 
	UFUNCTION()
	void HandleClicked();
 
private:
	
	int32 InvestmentId = 0;
};
