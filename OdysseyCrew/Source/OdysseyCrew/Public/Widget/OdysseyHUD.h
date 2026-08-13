// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "OdysseyHUD.generated.h"

class UOdysseyMainUserWidget;

UCLASS(Blueprintable, BlueprintType)
class ODYSSEYCREW_API AOdysseyHUD : public AHUD
{
	GENERATED_BODY()
	
public:
	
	UFUNCTION(BlueprintPure)
	UOdysseyMainUserWidget* GetOdysseyMainUserWidget() const { return OdysseyMainUserWidget; }
	
protected:
	
	virtual void BeginPlay() override;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "OdysseyHUD")
	TSubclassOf<UOdysseyMainUserWidget> OdysseyMainWidgetClass;
	
private:
	
	UPROPERTY()
	TObjectPtr<UOdysseyMainUserWidget> OdysseyMainUserWidget = nullptr;
};
