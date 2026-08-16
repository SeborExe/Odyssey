// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "OdysseyPlayerController.generated.h"

struct FInputActionValue;
class UOdysseyChangeBuildingWidget;
class AOdysseyHUD;
class AOdysseyBuildingsActor;
class UOdysseyFlatDetailsWidget;
class UOdysseyInvestmentSelectWidget;
class UInputAction;

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
 
	void HandlePrimaryPress(const FInputActionValue& Value);
	void HandleTouchPress(ETouchIndex::Type FingerIndex, FVector Location);
 
	UFUNCTION()
	void HandleFlatSelected(const FOdysseyFlat& Flat);
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Odyssey")
	TSubclassOf<UOdysseyFlatDetailsWidget> DetailsWidgetClass;
	
	//Inputs
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	TObjectPtr<UInputAction> IA_PrimaryPress;
 
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
	
	void LoadBuildings();
	UFUNCTION()
	void OnBuildingsLoaded();
	UFUNCTION()
	void BindToBuildings(AOdysseyBuildingsActor* Loaded);
	
	UPROPERTY()
	TWeakObjectPtr<AOdysseyHUD> OdysseyHUD;
 
	UPROPERTY(EditDefaultsOnly, Category="Odyssey") 
	TSoftObjectPtr<AOdysseyBuildingsActor> Buildings = nullptr;
	UPROPERTY() 
	TObjectPtr<UOdysseyFlatDetailsWidget> DetailsWidget = nullptr;
	
	UPROPERTY(VisibleAnywhere)
	int32 CurrentPositionIndex{0};
	
	float MoveToLocationX{0};
	bool bMove{false};
};
