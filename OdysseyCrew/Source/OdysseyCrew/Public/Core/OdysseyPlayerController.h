// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "OdysseyPlayerController.generated.h"

class UOdysseyChangeBuildingWidget;
class AOdysseyHUD;
class AOdysseyBuildingsActor;
class UOdysseyFlatDetailsWidget;
class UOdysseyInvestmentSelectWidget;

UCLASS(Blueprintable, BlueprintType)
class ODYSSEYCREW_API AOdysseyPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	
	void BindMainWidget();

protected:
	
	virtual void BeginPlay() override;
	virtual void Tick( float DeltaTime ) override;
	virtual void SetupInputComponent() override;
 
	void HandlePrimaryPress();
	void HandleTouchPress(ETouchIndex::Type FingerIndex, FVector Location);
 
	UFUNCTION()
	void HandleFlatSelected(const FOdysseyFlat& Flat);
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Odyssey")
	TSubclassOf<UOdysseyFlatDetailsWidget> DetailsWidgetClass;
 
private:
	
	void TrySelectFromHit(const FHitResult& Hit);
	
	UFUNCTION(BlueprintPure)
	AOdysseyHUD* GetOdysseyHUD();
	UFUNCTION(BlueprintPure)
	UOdysseyFlatDetailsWidget* GetFlatDetailsWidget();
	
	UOdysseyInvestmentSelectWidget* GetInvestmentSelectWidget();
	UFUNCTION() 
	void HandleInvestmentChosen(int32 InvestmentId);
	
	UOdysseyChangeBuildingWidget* GetOdysseyChangeBuildingWidgetNext();
	UOdysseyChangeBuildingWidget* GetOdysseyChangeBuildingWidgetPrev();
	UFUNCTION()
	void HandleChangeBuilding(bool bNext);
	
	UPROPERTY()
	TWeakObjectPtr<AOdysseyHUD> OdysseyHUD;
 
	UPROPERTY() 
	TObjectPtr<AOdysseyBuildingsActor> Buildings = nullptr;
	UPROPERTY() 
	TObjectPtr<UOdysseyFlatDetailsWidget> DetailsWidget = nullptr;
	
	UPROPERTY(VisibleAnywhere)
	int32 CurrentPositionIndex{0};
	
	float MoveToLocationX{0};
	bool bMove{false};
};
