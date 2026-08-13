// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "OdysseyChangeBuildingWidget.generated.h"

class UButton;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnChangeBuildingButtonClickedSignature, bool, bNext);

UCLASS()
class ODYSSEYCREW_API UOdysseyChangeBuildingWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	
	UPROPERTY(BlueprintAssignable)
	FOnChangeBuildingButtonClickedSignature OnChangeBuildingButtonClicked;
	
	UFUNCTION()
	void OnButtonClicked_Callback();
	
	void ChangeEnableState(bool bEnabled);
	void ChangeVisibility(bool bVisible);
	
protected:
	
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Button Props")
	bool bNext{false};
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UButton> ChangeBuildingButton;
};
