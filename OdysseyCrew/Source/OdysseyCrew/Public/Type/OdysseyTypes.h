// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "OdysseyTypes.generated.h"

// Inwestycja (osiedle) — tylko aktywne trafiają do prezentacji
USTRUCT(BlueprintType)
struct FOdysseyInvestment
{
    GENERATED_BODY()
 
    UPROPERTY(BlueprintReadOnly, Category = "Odyssey") 
    int32 Id{0};
    UPROPERTY(BlueprintReadOnly, Category = "Odyssey") 
    FName Name;
    UPROPERTY(BlueprintReadOnly, Category = "Odyssey") 
    FName City;
    UPROPERTY(BlueprintReadOnly, Category = "Odyssey") 
    FName Address;
    UPROPERTY(BlueprintReadOnly, Category = "Odyssey") 
    FName Description;
    UPROPERTY(BlueprintReadOnly, Category = "Odyssey") 
    FName Currency; // zakładam "PLN"
};
 
// Budynek wewnątrz inwestycji
USTRUCT(BlueprintType)
struct FOdysseyBuilding
{
    GENERATED_BODY()
 
    UPROPERTY(BlueprintReadOnly, Category = "Odyssey") 
    int32 Id{0};
    UPROPERTY(BlueprintReadOnly, Category = "Odyssey") 
    int32 InvestmentId = 0;
    UPROPERTY(BlueprintReadOnly, Category = "Odyssey") 
    FName Name;         // "Budynek A"
    UPROPERTY(BlueprintReadOnly, Category = "Odyssey") 
    FName Code;         // "A"
    UPROPERTY(BlueprintReadOnly, Category = "Odyssey") 
    int32   FloorsCount = 0;
    UPROPERTY(BlueprintReadOnly, Category = "Odyssey") 
    int32   OrderNumber = 0;
};
 
// Slownik statusow sprzedazy (available / reserved / sold)
USTRUCT(BlueprintType)
struct FOdysseyFlatStatus
{
    GENERATED_BODY()
 
    UPROPERTY(BlueprintReadOnly, Category = "Odyssey") 
    int32 Id = 0;
    UPROPERTY(BlueprintReadOnly, Category = "Odyssey") 
    FName Code;     // "available" | "reserved" | "sold"
    UPROPERTY(BlueprintReadOnly, Category = "Odyssey") 
    FName Label;  
    UPROPERTY(BlueprintReadOnly, Category = "Odyssey") 
    FString ColorHex;
    
    FLinearColor GetLinearColor() const
    {
        return FLinearColor::FromSRGBColor(FColor::FromHex(ColorHex));
    }
};

USTRUCT(BlueprintType)
struct FOdysseyExtra
{
    GENERATED_BODY()
 
    UPROPERTY(BlueprintReadOnly, Category = "Odyssey") 
    int32 Id{0};
    UPROPERTY(BlueprintReadOnly, Category = "Odyssey") 
    FName Name;
    UPROPERTY(BlueprintReadOnly, Category = "Odyssey") 
    FName Note;
};
 
// Mieszkanie — juz przefiltrowane (widoczne) i z rozwiazanymi NULL-ami
USTRUCT(BlueprintType)
struct FOdysseyFlat
{
    GENERATED_BODY()
 
    UPROPERTY(BlueprintReadOnly, Category = "Odyssey") 
    int32 Id = 0;
    UPROPERTY(BlueprintReadOnly, Category = "Odyssey") 
    int32 BuildingId = 0;
    UPROPERTY(BlueprintReadOnly, Category = "Odyssey") 
    int32 StatusId = 0;
    UPROPERTY(BlueprintReadOnly, Category = "Odyssey") 
    FName Name;        // "A.3.2"
    UPROPERTY(BlueprintReadOnly, Category = "Odyssey") 
    int32 EntryFloor = 0;
    UPROPERTY(BlueprintReadOnly, Category = "Odyssey") 
    int32 Floor = 0;
    UPROPERTY(BlueprintReadOnly, Category = "Odyssey") 
    int32 NumRooms = 0;
    UPROPERTY(BlueprintReadOnly, Category = "Odyssey") 
    float Area = 0.f;     // m^2
 
    // Ceny trzymane w bazie w GROSZACH. NULL = brak ceny do publicznej prezentacji
    // Rozrozniamy "brak ceny" od "cena = 0" -> flaga bHasPrice
    UPROPERTY(BlueprintReadOnly, Category = "Odyssey") 
    bool bHasPrice = false;
    UPROPERTY(BlueprintReadOnly, Category = "Odyssey") 
    int64 PriceGrosze = 0;
    UPROPERTY(BlueprintReadOnly, Category = "Odyssey") 
    int64 PriceSqmGrosze = 0;
    
    UPROPERTY(BlueprintReadOnly, Category = "Odyssey") 
    bool bHasMesh = false;
    UPROPERTY(BlueprintReadOnly, Category = "Odyssey") 
    FString MeshId;
 
    UPROPERTY(BlueprintReadOnly, Category = "Odyssey") 
    FString Description;
    UPROPERTY(BlueprintReadOnly, Category = "Odyssey") 
    TArray<FOdysseyExtra> Extras;
 
    // Dwupoziomowe: Floor == EntryFloor + 1. Box ma wtedy podwojna wysokosc
    bool  IsDuplex()  const { return Floor > EntryFloor; }
    int32 FloorSpan() const { return Floor - EntryFloor + 1; }
 
    // Ceny w zlotych
    double PriceZl()    const { return PriceGrosze    / 100.0; }
    double PriceSqmZl() const { return PriceSqmGrosze / 100.0; }
};