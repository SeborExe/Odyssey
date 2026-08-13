// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "OdysseyTypes.generated.h"

// Inwestycja (osiedle) — tylko aktywne trafiają do prezentacji.
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
    FName Currency; // zwykle "PLN"
};
 
// Budynek wewnątrz inwestycji.
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
    int32   FloorsCount = 0; // kondygnacje 0 .. FloorsCount-1
    UPROPERTY(BlueprintReadOnly, Category = "Odyssey") 
    int32   OrderNumber = 0; // kolejnosc prezentacji
};
 
// Slownik statusow sprzedazy (available / reserved / sold).
USTRUCT(BlueprintType)
struct FOdysseyFlatStatus
{
    GENERATED_BODY()
 
    UPROPERTY(BlueprintReadOnly, Category = "Odyssey") 
    int32 Id = 0;
    UPROPERTY(BlueprintReadOnly, Category = "Odyssey") 
    FName Code;     // "available" | "reserved" | "sold"
    UPROPERTY(BlueprintReadOnly, Category = "Odyssey") 
    FName Label;    // label_pl, np. "Wolne"
    UPROPERTY(BlueprintReadOnly, Category = "Odyssey") 
    FString ColorHex; // np. "#4CAF50"
 
    // Kolor gotowy do materialu/UI. Hex jest w przestrzeni sRGB,
    // wiec konwertujemy poprawnie do liniowej.
    FLinearColor GetLinearColor() const
    {
        return FLinearColor::FromSRGBColor(FColor::FromHex(ColorHex));
    }
};
 
// Dodatek (balkon, ogrodek, komorka...) z opcjonalnym opisem per mieszkanie.
USTRUCT(BlueprintType)
struct FOdysseyExtra
{
    GENERATED_BODY()
 
    UPROPERTY(BlueprintReadOnly, Category = "Odyssey") 
    int32 Id{0};
    UPROPERTY(BlueprintReadOnly, Category = "Odyssey") 
    FName Name; // "Balkon"
    UPROPERTY(BlueprintReadOnly, Category = "Odyssey") 
    FName Note; // "Balkon 6.2 m2" lub puste
};
 
// Mieszkanie — juz przefiltrowane (widoczne) i z rozwiazanymi NULL-ami.
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
    int32 EntryFloor = 0; // kondygnacja wejscia
    UPROPERTY(BlueprintReadOnly, Category = "Odyssey") 
    int32 Floor = 0;      // najwyzsza zajmowana kondygnacja
    UPROPERTY(BlueprintReadOnly, Category = "Odyssey") 
    int32 NumRooms = 0;
    UPROPERTY(BlueprintReadOnly, Category = "Odyssey") 
    float Area = 0.f;     // m2
 
    // Ceny trzymane w bazie w GROSZACH. NULL = brak ceny do publicznej prezentacji.
    // Rozrozniamy "brak ceny" od "cena = 0" flaga bHasPrice.
    UPROPERTY(BlueprintReadOnly, Category = "Odyssey") 
    bool bHasPrice = false;
    UPROPERTY(BlueprintReadOnly, Category = "Odyssey") 
    int64 PriceGrosze = 0;
    UPROPERTY(BlueprintReadOnly, Category = "Odyssey") 
    int64 PriceSqmGrosze = 0;
 
    // mesh_id moze byc NULL (lokal bez reprezentacji 3D). W tym zadaniu
    // geometrie generujemy z danych, wiec to pole jest w zasadzie informacyjne.
    UPROPERTY(BlueprintReadOnly, Category = "Odyssey") 
    bool bHasMesh = false;
    UPROPERTY(BlueprintReadOnly, Category = "Odyssey") 
    FString MeshId;
 
    UPROPERTY(BlueprintReadOnly, Category = "Odyssey") 
    FString Description;
    UPROPERTY(BlueprintReadOnly, Category = "Odyssey") 
    TArray<FOdysseyExtra> Extras;
 
    // Dwupoziomowe: Floor == EntryFloor + 1. Box ma wtedy podwojna wysokosc.
    bool  IsDuplex()  const { return Floor > EntryFloor; }
    int32 FloorSpan() const { return Floor - EntryFloor + 1; }
 
    // Ceny w zlotych (do formatowania w warstwie prezentacji).
    double PriceZl()    const { return PriceGrosze    / 100.0; }
    double PriceSqmZl() const { return PriceSqmGrosze / 100.0; }
};