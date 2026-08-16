// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "OdysseyInvestmentSelectWidget.generated.h"

class UOdysseyInvestmentEntryWidget;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInvestmentChosen, int32, InvestmentId);

UCLASS()
class ODYSSEYCREW_API UOdysseyInvestmentSelectWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintAssignable, Category = "Odyssey")
	FOnInvestmentChosen OnInvestmentChosen;
	
	UFUNCTION(BlueprintCallable, Category = "Odyssey")
	void Refresh();
 
	UFUNCTION(BlueprintCallable, Category = "Odyssey")
	void Show();
 
	UFUNCTION(BlueprintCallable, Category = "Odyssey")
	void Hide();
 
protected:
	virtual void NativeConstruct() override;
	
	UPROPERTY(meta = (BindWidget))
	UPanelWidget* ListContainer = nullptr;
	
	UPROPERTY(EditAnywhere, Category = "Odyssey")
	TSubclassOf<UOdysseyInvestmentEntryWidget> EntryWidgetClass;
 
	UFUNCTION()
	void HandleEntryChosen(int32 InvestmentId);
};
