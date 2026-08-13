// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "OdysseyMainUserWidget.generated.h"

class UOdysseyChangeBuildingWidget;
class UOdysseyFlatDetailsWidget;
class UOdysseyInvestmentSelectWidget;

UCLASS()
class ODYSSEYCREW_API UOdysseyMainUserWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	
	UFUNCTION(BlueprintPure, Category = "Getters")
	UOdysseyFlatDetailsWidget* GetFlatDetailsWidget() const { return WBP_OdysseyFlatDetails; }
	UFUNCTION(BlueprintPure, Category = "Getters")
	UOdysseyInvestmentSelectWidget* GetInvestmentSelectWidget() const { return WBP_OdysseyInvestmentSelect; }
	UFUNCTION(BlueprintPure, Category = "Getters")
	UOdysseyChangeBuildingWidget* GetChangeBuildingNext() const { return WBP_OdysseyChangeBuildingNext; }
	UFUNCTION(BlueprintPure, Category = "Getters")
	UOdysseyChangeBuildingWidget* GetChangeBuildingPrevious() const { return WBP_OdysseyChangeBuildingPrevious; }
	
protected:
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UOdysseyFlatDetailsWidget> WBP_OdysseyFlatDetails;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UOdysseyInvestmentSelectWidget> WBP_OdysseyInvestmentSelect;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UOdysseyChangeBuildingWidget> WBP_OdysseyChangeBuildingNext;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UOdysseyChangeBuildingWidget> WBP_OdysseyChangeBuildingPrevious;
};
