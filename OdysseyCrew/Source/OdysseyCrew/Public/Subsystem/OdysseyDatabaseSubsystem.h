// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SQLiteDatabase.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Type/OdysseyTypes.h"
#include "OdysseyDatabaseSubsystem.generated.h"

UCLASS()
class ODYSSEYCREW_API UOdysseyDatabaseSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
 
	//Otwiera baze w trybie READ-ONLY. false = plik nie istnieje
	UFUNCTION(BlueprintCallable, Category = "Odyssey|DB")
	bool OpenDatabase(const FString& AbsolutePath);
	
	UFUNCTION(BlueprintCallable, Category = "Odyssey|DB")
	bool EnsureOpen();
 
	UFUNCTION(BlueprintCallable, Category = "Odyssey|DB")
	bool IsOpen() const;
 
	// Tylko aktywne inwestycje
	UFUNCTION(BlueprintCallable, Category = "Odyssey|DB")
	TArray<FOdysseyInvestment> GetInvestments();
 
	// Aktywne budynki aktywnej inwestycji
	UFUNCTION(BlueprintCallable, Category = "Odyssey|DB")
	TArray<FOdysseyBuilding> GetBuildings(int32 InvestmentId);
 
	/**
	 * Widoczne mieszkania budynku. Kaskada: inwestycja + budynek + mieszkanie
	 * musza byc aktywne. Extras dolaczane per mieszkanie.
	 */
	UFUNCTION(BlueprintCallable, Category = "Odyssey|DB")
	TArray<FOdysseyFlat> GetFlats(int32 BuildingId);
	
	UFUNCTION(BlueprintCallable, Category = "Odyssey|DB")
	bool GetStatus(int32 StatusId, FOdysseyFlatStatus& OutStatus);
	
	UFUNCTION(BlueprintCallable, Category = "Odyssey|DB")
	TArray<FOdysseyFlatStatus> GetAllStatuses();
 
private:
	void EnsureStatusesLoaded();
	TArray<FOdysseyExtra> LoadExtrasForFlat(int32 FlatId);
	
	UPROPERTY()
	FString DatabaseRelativePath = TEXT("Data/odyssey_demo.db");
 
	TUniquePtr<FSQLiteDatabase> Db;
	TMap<int32, FOdysseyFlatStatus> StatusCache;
	bool bStatusesLoaded = false;
};
