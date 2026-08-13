// Fill out your copyright notice in the Description page of Project Settings.

#include "Actor/OdysseyBuildingsActor.h"

#include "Components/InstancedStaticMeshComponent.h"
#include "Subsystem/OdysseyDatabaseSubsystem.h"

AOdysseyBuildingsActor::AOdysseyBuildingsActor()
{
    PrimaryActorTick.bCanEverTick = false;
 
    Flats = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("Flats"));
    RootComponent = Flats;
 
    // Domyslny szescian silnika: 100x100x100 uu, pivot w srodku.
    static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMesh(TEXT("/Engine/BasicShapes/Cube.Cube"));
    if (CubeMesh.Succeeded())
    {
        Flats->SetStaticMesh(CubeMesh.Object);
    }
 
    Flats->NumCustomDataFloats = 4; // R, G, B, Highlight
 
    // Kolizja tylko do trace'a (klikanie), bez fizyki.
    Flats->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    Flats->SetCollisionResponseToAllChannels(ECR_Block);
    Flats->SetMobility(EComponentMobility::Movable);
}
 
void AOdysseyBuildingsActor::BeginPlay()
{
    Super::BeginPlay();
 
    if (FlatMaterial)
    {
        Flats->SetMaterial(0, FlatMaterial);
    }
}
 
float AOdysseyBuildingsActor::ComputeFlatWidth(float AreaSqm) const
{
    return FMath::Clamp(AreaSqm * WidthPerSqm, MinFlatWidth, MaxFlatWidth);
}
 
void AOdysseyBuildingsActor::Rebuild()
{
    Flats->ClearInstances();
    InstanceFlats.Reset();
    SelectedInstance = INDEX_NONE;
    BuildingLocalBounds.Reset();
 
    UGameInstance* GI = GetGameInstance();
    UOdysseyDatabaseSubsystem* DB = GI ? GI->GetSubsystem<UOdysseyDatabaseSubsystem>() : nullptr;
    
    if (!DB) return;
    DB->EnsureOpen();
    if (!DB->IsOpen()) return;
 
    const TArray<FOdysseyBuilding> Buildings = DB->GetBuildings(InvestmentId);
 
    // Budynki obok siebie wzdluz X (wyrownane do lewej).
    float BuildingOriginX = 0.f;
    for (const FOdysseyBuilding& B : Buildings)
    {
        const TArray<FOdysseyFlat> BuildingFlats = DB->GetFlats(B.Id);
 
        // Grupowanie po kondygnacji wejscia; klucze posortowane rosnaco,
        // bo duplexy rezerwuja kolumne na piEtrze WYZEJ (przetwarzamy od dolu).
        TMap<int32, TArray<const FOdysseyFlat*>> ByFloor;
        for (const FOdysseyFlat& F : BuildingFlats)
        {
            ByFloor.FindOrAdd(F.EntryFloor).Add(&F);
        }
 
        TArray<int32> FloorKeys;
        ByFloor.GetKeys(FloorKeys);
        FloorKeys.Sort();
 
        // floor -> zajete przedzialy X (od duplexow z kondygnacji nizej).
        TMap<int32, TArray<TPair<float, float>>> Reserved;
        float BuildingWidth = 0.f;
 
        for (int32 FloorNo : FloorKeys)
        {
            TArray<TPair<float, float>>* Res = Reserved.Find(FloorNo);
            if (Res)
            {
                Res->Sort([](const TPair<float, float>& A, const TPair<float, float>& B)
                {
                    return A.Key < B.Key;
                });
            }
 
            float CursorX = BuildingOriginX;
 
            for (const FOdysseyFlat* F : ByFloor[FloorNo])
            {
                const float W = ComputeFlatWidth(F->Area);
 
                // Omin kolumny zajete przez duplexy z kondygnacji nizej.
                if (Res)
                {
                    for (const TPair<float, float>& R : *Res)
                    {
                        const bool bOverlap = (CursorX < R.Value) && (CursorX + W > R.Key);
                        if (bOverlap)
                        {
                            CursorX = R.Value + FlatGap;
                        }
                    }
                }
 
                const float H = StoreyHeight * FMath::Max(1, F->FloorSpan()); // duplex = 2 kondygnacje
 
                const float CenterX = CursorX + W * 0.5f;
                const float CenterZ = FloorNo * StoreyHeight + H * 0.5f;
 
                FTransform T;
                T.SetLocation(FVector(CenterX, 0.f, CenterZ));
                T.SetScale3D(FVector(W / 100.f, FlatDepth / 100.f, H / 100.f)); // szescian bazowy = 100 uu
 
                const int32 Idx = Flats->AddInstance(T);
                InstanceFlats.Add(*F); // kopia; index zgodny z ISM
 
                // Duplex rezerwuje swoja kolumne na piEtrze wyzej.
                if (F->IsDuplex())
                {
                    Reserved.FindOrAdd(FloorNo + 1).Add(TPair<float, float>(CursorX, CursorX + W));
                }
 
                // Kolor statusu (linear) -> custom data. Highlight = 0.
                FLinearColor C = FLinearColor::Gray;
                FOdysseyFlatStatus St;
                if (DB->GetStatus(F->StatusId, St))
                {
                    C = St.GetLinearColor();
                }
                Flats->SetCustomDataValue(Idx, 0, C.R);
                Flats->SetCustomDataValue(Idx, 1, C.G);
                Flats->SetCustomDataValue(Idx, 2, C.B);
                Flats->SetCustomDataValue(Idx, 3, 0.f);
 
                CursorX += W + FlatGap;
            }
 
            BuildingWidth = FMath::Max(BuildingWidth, CursorX - BuildingOriginX);
            
        }
 
        const float TopZ = B.FloorsCount * StoreyHeight;
        const FVector BMin(BuildingOriginX,               -FlatDepth * 0.5f, 0.f);
        const FVector BMax(BuildingOriginX + BuildingWidth, FlatDepth * 0.5f, TopZ);
        BuildingLocalBounds.Add(FBox(BMin, BMax));;
        
        BuildingOriginX += BuildingWidth + BuildingGap;
    }
 
    Flats->MarkRenderStateDirty();
}

bool AOdysseyBuildingsActor::GetBuildingCenter(int32 BuildingIndex, FVector& OutWorldCenter) const
{
    if (!BuildingLocalBounds.IsValidIndex(BuildingIndex))
        return false;

    OutWorldCenter = Flats->GetComponentTransform()
        .TransformPosition(BuildingLocalBounds[BuildingIndex].GetCenter());
    return true;
}

bool AOdysseyBuildingsActor::SelectByInstanceIndex(int32 InstanceIndex)
{
    if (!InstanceFlats.IsValidIndex(InstanceIndex))
    {
        return false;
    }
 
    if (InstanceFlats.IsValidIndex(SelectedInstance))
    {
        Flats->SetCustomDataValue(SelectedInstance, 3, 0.f, false);
    }
 
    SelectedInstance = InstanceIndex;
    Flats->SetCustomDataValue(InstanceIndex, 3, 1.f, true);
 
    OnFlatSelected.Broadcast(InstanceFlats[InstanceIndex]);
    return true;
}
 
void AOdysseyBuildingsActor::ClearSelection()
{
    if (InstanceFlats.IsValidIndex(SelectedInstance))
    {
        Flats->SetCustomDataValue(SelectedInstance, 3, 0.f, true);
    }
    
    SelectedInstance = INDEX_NONE;
}