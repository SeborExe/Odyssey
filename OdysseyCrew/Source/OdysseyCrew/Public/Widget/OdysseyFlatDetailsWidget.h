// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "OdysseyFlatDetailsWidget.generated.h"

class UTextBlock;

UCLASS(Blueprintable, BlueprintType)
class ODYSSEYCREW_API UOdysseyFlatDetailsWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:

	UFUNCTION(BlueprintCallable, Category = "Odyssey")
	void SetFlat(const FOdysseyFlat& Flat);
	
	UFUNCTION(BlueprintCallable, Category = "Odyssey")
	void Clear();
	
protected:

	UPROPERTY(meta = (BindWidgetOptional)) 
	UTextBlock* NameText = nullptr;
	UPROPERTY(meta = (BindWidgetOptional)) 
	UTextBlock* StatusText = nullptr;
	UPROPERTY(meta = (BindWidgetOptional)) 
	UTextBlock* RoomsAreaText = nullptr;
	UPROPERTY(meta = (BindWidgetOptional)) 
	UTextBlock* PriceText = nullptr;
	UPROPERTY(meta = (BindWidgetOptional)) 
	UTextBlock* PriceSqmText = nullptr;
	UPROPERTY(meta = (BindWidgetOptional)) 
	UTextBlock* DescriptionText = nullptr;
	UPROPERTY(meta = (BindWidgetOptional)) 
	UTextBlock* ExtrasText = nullptr;
 
private:
	
	static FString FormatGroszeToPln(int64 Grosze); // 113714675 -> "1 137 146,75"
	static FString FormatArea(float AreaSqm);        // 65.89 -> "65,89"
	static FString RoomsWord(int32 NumRooms);  
};
