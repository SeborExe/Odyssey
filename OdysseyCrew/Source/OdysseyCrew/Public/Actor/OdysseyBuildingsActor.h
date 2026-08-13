// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "OdysseyBuildingsActor.generated.h"

struct FOdysseyFlat;
class UInstancedStaticMeshComponent;
class UMaterialInterface;
 
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnFlatSelected, const FOdysseyFlat&, Flat);

UCLASS()
class ODYSSEYCREW_API AOdysseyBuildingsActor : public AActor
{
	GENERATED_BODY()

	public:
    AOdysseyBuildingsActor();
 
    /** Ktora inwestycja. Na razie z edytora UI wyboru w kolejnym kroku. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Odyssey")
    int32 InvestmentId = 1;
 
    /** Material czytajacy PerInstanceCustomData (0..2 = RGB, 3 = highlight). */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Odyssey")
    UMaterialInterface* FlatMaterial = nullptr;
 
    // --- wymiary bryl, w jednostkach UE (cm) ---
    UPROPERTY(EditAnywhere, Category = "Odyssey|Layout") float StoreyHeight = 300.f;
    UPROPERTY(EditAnywhere, Category = "Odyssey|Layout") float FlatDepth    = 400.f;
    UPROPERTY(EditAnywhere, Category = "Odyssey|Layout") float FlatGap      = 40.f;
    UPROPERTY(EditAnywhere, Category = "Odyssey|Layout") float BuildingGap  = 800.f;
    UPROPERTY(EditAnywhere, Category = "Odyssey|Layout") float WidthPerSqm  = 7.f;
    UPROPERTY(EditAnywhere, Category = "Odyssey|Layout") float MinFlatWidth = 180.f;
    UPROPERTY(EditAnywhere, Category = "Odyssey|Layout") float MaxFlatWidth = 900.f;
 
    /** Klikniecie mieszkania -> panel szczegolow (podepniemy w kolejnym kroku). */
    UPROPERTY(BlueprintAssignable, Category = "Odyssey")
    FOnFlatSelected OnFlatSelected;
 
    /** Wolane po trace: true + broadcast, jesli instancja to mieszkanie. */
    UFUNCTION(BlueprintCallable, Category = "Odyssey")
    bool SelectByInstanceIndex(int32 InstanceIndex);
 
    UFUNCTION(BlueprintCallable, Category = "Odyssey")
    void ClearSelection();
 
    /** Przebudowa geometrii (np. po zmianie InvestmentId). */
    UFUNCTION(BlueprintCallable, Category = "Odyssey")
    void Rebuild();
    
    UFUNCTION(BlueprintCallable, Category = "Odyssey")
    bool GetBuildingCenter(int32 BuildingId, FVector& OutWorldCenter) const;
    UFUNCTION(BlueprintPure, Category = "Odyssey")
    int32 GetBuildingsNum() const { return  BuildingLocalBounds.Num(); }
 
protected:
    virtual void BeginPlay() override;
 
private:
    float ComputeFlatWidth(float AreaSqm) const;
 
    UPROPERTY(VisibleAnywhere, Category = "Odyssey")
    UInstancedStaticMeshComponent* Flats = nullptr;
 
    /** instancja ISM -> mieszkanie (indeksy zgodne). */
    UPROPERTY()
    TArray<FOdysseyFlat> InstanceFlats;
    TArray<FBox> BuildingLocalBounds; 
 
    int32 SelectedInstance = INDEX_NONE;
};
