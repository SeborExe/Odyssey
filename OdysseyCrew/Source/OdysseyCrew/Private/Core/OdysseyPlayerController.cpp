// Fill out your copyright notice in the Description page of Project Settings.

#include "Core/OdysseyPlayerController.h"

#include "Actor/OdysseyBuildingsActor.h"
#include "Kismet/GameplayStatics.h"
#include "Widget/OdysseyChangeBuildingWidget.h"
#include "Widget/OdysseyFlatDetailsWidget.h"
#include "Widget/OdysseyHUD.h"
#include "Widget/OdysseyInvestmentSelectWidget.h"
#include "Widget/OdysseyMainUserWidget.h"

void AOdysseyPlayerController::BeginPlay()
{
	Super::BeginPlay();
	SetActorTickEnabled(false);
	
	bShowMouseCursor   = true;
	bEnableClickEvents = true;
	bEnableTouchEvents = true;
 
	// Aktor z bryłami (na scenie jest jeden).
	Buildings = Cast<AOdysseyBuildingsActor>(
		UGameplayStatics::GetActorOfClass(this, AOdysseyBuildingsActor::StaticClass()));
 
	if (Buildings)
	{
		Buildings->OnFlatSelected.AddDynamic(this, &AOdysseyPlayerController::HandleFlatSelected);
	}
}

void AOdysseyPlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (bMove && GetPawn())
	{
		FVector Destination = FVector(MoveToLocationX, GetPawn()->GetActorLocation().Y, GetPawn()->GetActorLocation().Z);
		FVector NextLoc = FMath::Lerp(GetPawn()->GetActorLocation(), Destination, DeltaTime);
		GetPawn()->SetActorLocation(NextLoc);
		
		if (GetPawn()->GetActorLocation().X == MoveToLocationX)
		{
			bMove = false;
			SetActorTickEnabled(false);
		}
	}
}

void AOdysseyPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	
	// Klasyczny binding dziala bez zakladania Enhanced Input assetow.
	InputComponent->BindKey(EKeys::LeftMouseButton, IE_Pressed, this,
		&AOdysseyPlayerController::HandlePrimaryPress);
	InputComponent->BindTouch(IE_Pressed, this,
		&AOdysseyPlayerController::HandleTouchPress);
}

void AOdysseyPlayerController::HandlePrimaryPress()
{
	FHitResult Hit;
	GetHitResultUnderCursor(ECC_Visibility, false, Hit);
	TrySelectFromHit(Hit);
}

void AOdysseyPlayerController::HandleTouchPress(ETouchIndex::Type FingerIndex, FVector Location)
{
	FHitResult Hit;
	GetHitResultUnderFinger(FingerIndex, ECC_Visibility, false, Hit);
	TrySelectFromHit(Hit);
}

void AOdysseyPlayerController::HandleFlatSelected(const FOdysseyFlat& Flat)
{
	if (GetFlatDetailsWidget())
	{
		DetailsWidget.Get()->SetFlat(Flat);
	}
}

void AOdysseyPlayerController::TrySelectFromHit(const FHitResult& Hit)
{
	// Trafienie w aktora z bryłami? Hit.Item = indeks instancji ISM.
	if (AOdysseyBuildingsActor* Hitted = Cast<AOdysseyBuildingsActor>(Hit.GetActor()))
	{
		if (Hitted->SelectByInstanceIndex(Hit.Item))
		{
			return; // panel pokaze HandleFlatSelected
		}
	}
 
	// Klik w pustke / poza mieszkaniem -> schowaj panel.
	if (Buildings)
	{
		Buildings->ClearSelection();
	}
	if (GetFlatDetailsWidget())
	{
		DetailsWidget.Get()->Clear();
	}
}

void AOdysseyPlayerController::BindMainWidget()
{
	if (UOdysseyInvestmentSelectWidget* Sel = GetInvestmentSelectWidget())
	{
		if (!Sel->OnInvestmentChosen.IsAlreadyBound(this, &AOdysseyPlayerController::HandleInvestmentChosen))
			Sel->OnInvestmentChosen.AddDynamic(this, &AOdysseyPlayerController::HandleInvestmentChosen);
	}
	
	if (UOdysseyChangeBuildingWidget* NextBuilding = GetOdysseyChangeBuildingWidgetNext())
	{
		if (!NextBuilding->OnChangeBuildingButtonClicked.IsAlreadyBound(this, &ThisClass::HandleChangeBuilding))
			NextBuilding->OnChangeBuildingButtonClicked.AddDynamic(this, &ThisClass::HandleChangeBuilding);
	}
	
	if (UOdysseyChangeBuildingWidget* PreviousBuilding = GetOdysseyChangeBuildingWidgetPrev())
	{
		if (!PreviousBuilding->OnChangeBuildingButtonClicked.IsAlreadyBound(this, &ThisClass::HandleChangeBuilding))
			PreviousBuilding->OnChangeBuildingButtonClicked.AddDynamic(this, &ThisClass::HandleChangeBuilding);
	}
}

AOdysseyHUD* AOdysseyPlayerController::GetOdysseyHUD()
{
	if (!OdysseyHUD.IsValid())
	{
		OdysseyHUD = Cast<AOdysseyHUD>(GetHUD());
	}
	
	return OdysseyHUD.Get();
}

UOdysseyFlatDetailsWidget* AOdysseyPlayerController::GetFlatDetailsWidget()
{
	if (!IsValid(DetailsWidget))
	{
		if (GetOdysseyHUD() && GetOdysseyHUD()->GetOdysseyMainUserWidget())
		{
			DetailsWidget = GetOdysseyHUD()->GetOdysseyMainUserWidget()->GetFlatDetailsWidget();
		}
	}
	
	return DetailsWidget.Get();
}

UOdysseyInvestmentSelectWidget* AOdysseyPlayerController::GetInvestmentSelectWidget()
{
	if (GetOdysseyHUD() && GetOdysseyHUD()->GetOdysseyMainUserWidget())
		return GetOdysseyHUD()->GetOdysseyMainUserWidget()->GetInvestmentSelectWidget();
	
	return nullptr;
}

void AOdysseyPlayerController::HandleInvestmentChosen(int32 InvestmentId)
{
	if (Buildings)
	{
		Buildings->InvestmentId = InvestmentId;
		Buildings->Rebuild();
	}
	
	if (GetFlatDetailsWidget()) 
		DetailsWidget.Get()->Clear();
	
	if (GetOdysseyChangeBuildingWidgetNext() && GetOdysseyChangeBuildingWidgetPrev())
	{
		GetOdysseyChangeBuildingWidgetNext()->ChangeVisibility(true);
		GetOdysseyChangeBuildingWidgetPrev()->ChangeVisibility(true);
		
		CurrentPositionIndex = -1;
		HandleChangeBuilding(true);
	}
}

UOdysseyChangeBuildingWidget* AOdysseyPlayerController::GetOdysseyChangeBuildingWidgetNext()
{
	if (GetOdysseyHUD() && GetOdysseyHUD()->GetOdysseyMainUserWidget())
		return GetOdysseyHUD()->GetOdysseyMainUserWidget()->GetChangeBuildingNext();
	
	return nullptr;
}

UOdysseyChangeBuildingWidget* AOdysseyPlayerController::GetOdysseyChangeBuildingWidgetPrev()
{
	if (GetOdysseyHUD() && GetOdysseyHUD()->GetOdysseyMainUserWidget())
		return GetOdysseyHUD()->GetOdysseyMainUserWidget()->GetChangeBuildingPrevious();
	
	return nullptr;
}

void AOdysseyPlayerController::HandleChangeBuilding(bool bNext)
{
	CurrentPositionIndex = bNext ? CurrentPositionIndex + 1 : CurrentPositionIndex - 1;
	if (CurrentPositionIndex == 0)
	{
		GetOdysseyChangeBuildingWidgetPrev()->SetIsEnabled(false);
	}
	else
	{
		GetOdysseyChangeBuildingWidgetPrev()->SetIsEnabled(true);
	}
	
	FVector BuildingLoc;
	if (Buildings->GetBuildingCenter(CurrentPositionIndex, BuildingLoc))
	{
		MoveToLocationX = BuildingLoc.X;
		bMove = true;
		SetActorTickEnabled(true);
	}
	
	FVector NextBuildingLoc;
	int32 NextIndex = CurrentPositionIndex + 1;
	if (!Buildings->GetBuildingCenter(NextIndex, NextBuildingLoc))
	{
		GetOdysseyChangeBuildingWidgetNext()->SetIsEnabled(false);
	}
	else
	{
		GetOdysseyChangeBuildingWidgetNext()->SetIsEnabled(true);
	}
}
