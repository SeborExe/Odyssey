// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/OdysseyFlatDetailsWidget.h"

#include "Components/TextBlock.h"
#include "Subsystem/OdysseyDatabaseSubsystem.h"
#include "Type/OdysseyTypes.h"

void UOdysseyFlatDetailsWidget::SetFlat(const FOdysseyFlat& Flat)
{
	if (NameText)
	{
		NameText->SetText(FText::FromString(Flat.Name.ToString()));
	}
 
	if (StatusText)
	{
		FOdysseyFlatStatus St;
		bool bResolved = false;
		if (UGameInstance* GI = GetGameInstance())
		{
			if (UOdysseyDatabaseSubsystem* DB = GI->GetSubsystem<UOdysseyDatabaseSubsystem>())
			{
				bResolved = DB->GetStatus(Flat.StatusId, St);
			}
		}
		if (bResolved)
		{
			StatusText->SetText(FText::FromString(St.Label.ToString()));
			StatusText->SetColorAndOpacity(FSlateColor(St.GetLinearColor()));
		}
	}
 
	if (RoomsAreaText)
	{
		const FString S = FString::Printf(TEXT("%d %s \u00B7 %s m\u00B2"),
			Flat.NumRooms, *RoomsWord(Flat.NumRooms), *FormatArea(Flat.Area));
		RoomsAreaText->SetText(FText::FromString(S));
	}
 
	if (PriceText)
	{
		PriceText->SetText(FText::FromString(
			Flat.bHasPrice ? FormatGroszeToPln(Flat.PriceGrosze) + TEXT(" z\u0142")
						   : TEXT("Cena na zapytanie")));
	}
 
	if (PriceSqmText)
	{
		PriceSqmText->SetText(Flat.bHasPrice
			? FText::FromString(FormatGroszeToPln(Flat.PriceSqmGrosze) + TEXT(" z\u0142/m\u00B2"))
			: FText::GetEmpty());
	}
 
	if (DescriptionText)
	{
		DescriptionText->SetText(FText::FromString(Flat.Description));
		DescriptionText->SetVisibility(Flat.Description.IsEmpty() ? ESlateVisibility::Collapsed : ESlateVisibility::Visible);
	}
 
	if (ExtrasText)
	{
		TArray<FString> Lines;
		for (const FOdysseyExtra& E : Flat.Extras)
		{
			Lines.Add(E.Note.IsNone() ? E.Name.ToString()
									   : FString::Printf(TEXT("%s \u2014 %s"), *E.Name.ToString(), *E.Note.ToString()));
		}
		
		ExtrasText->SetText(FText::FromString(FString::Join(Lines, TEXT("\n"))));
	}
 
	SetVisibility(ESlateVisibility::Visible);
}

void UOdysseyFlatDetailsWidget::Clear()
{
	SetVisibility(ESlateVisibility::Collapsed);
}

FString UOdysseyFlatDetailsWidget::FormatGroszeToPln(int64 Grosze)
{
	const bool bNeg = Grosze < 0;
	const int64 Abs = bNeg ? -Grosze : Grosze;
	const int64 Zl  = Abs / 100;
	const int32 Gr  = static_cast<int32>(Abs % 100);
 
	const FString ZlStr = FString::Printf(TEXT("%lld"), Zl);
 
	// Grupowanie czesci calkowitej spacjami co 3 cyfry.
	FString Grouped;
	int32 Count = 0;
	for (int32 i = ZlStr.Len() - 1; i >= 0; --i)
	{
		Grouped = FString::Chr(ZlStr[i]) + Grouped;
		if (++Count % 3 == 0 && i > 0)
		{
			Grouped = TEXT(" ") + Grouped;
		}
	}
 
	return FString::Printf(TEXT("%s%s,%02d"), bNeg ? TEXT("-") : TEXT(""), *Grouped, Gr);
}

FString UOdysseyFlatDetailsWidget::FormatArea(float AreaSqm)
{
	FString S = FString::Printf(TEXT("%.2f"), AreaSqm);
	S.ReplaceInline(TEXT("."), TEXT(","));
	return S;
}

FString UOdysseyFlatDetailsWidget::RoomsWord(int32 NumRooms)
{
	if (NumRooms == 1)
	{
		return TEXT("pok\u00F3j");
	}
 
	const int32 Last  = NumRooms % 10;
	const int32 Last2 = NumRooms % 100;
	if (Last >= 2 && Last <= 4 && !(Last2 >= 12 && Last2 <= 14))
	{
		return TEXT("pokoje");
	}
	return TEXT("pokoi");
}
