// Fill out your copyright notice in the Description page of Project Settings.

#include "Subsystem/OdysseyDatabaseSubsystem.h"

#include "SQLiteDatabase.h"

void UOdysseyDatabaseSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
}

void UOdysseyDatabaseSubsystem::Deinitialize()
{
	if (Db.IsValid())
	{
		Db->Close();
		Db.Reset();
	}
	
	Super::Deinitialize();
}

bool UOdysseyDatabaseSubsystem::OpenDatabase(const FString& AbsolutePath)
{
	if (Db.IsValid())
	{
		Db->Close();
		Db.Reset();
	}
	
	Db = MakeUnique<FSQLiteDatabase>();
	
	// READ-ONLY: nic nie zapisujemy, plik nie zostanie zablokowany do zapisu
	if (!Db->Open(*AbsolutePath, ESQLiteDatabaseOpenMode::ReadOnly))
	{
		UE_LOG(LogTemp, Error, TEXT("[Odyssey] Nie udalo sie otworzyc bazy: %s"), *AbsolutePath);
		Db.Reset();
		return false;
	}
 
	bStatusesLoaded = false; // wymus ponowne wczytanie slownika przy nowej bazie
	return true;
}

bool UOdysseyDatabaseSubsystem::EnsureOpen()
{
	if (IsOpen()) return true;
	return OpenDatabase(FPaths::ProjectContentDir() / DatabaseRelativePath);
}

bool UOdysseyDatabaseSubsystem::IsOpen() const
{
	return Db.IsValid() && Db->IsValid();
}

TArray<FOdysseyInvestment> UOdysseyDatabaseSubsystem::GetInvestments()
{
	TArray<FOdysseyInvestment> Result;
	if (!IsOpen())
	{
		return Result;
	}
 
	FSQLitePreparedStatement Stmt;
	Stmt.Create(*Db, TEXT(
		"SELECT id, name, city, address, description, currency "
		"FROM investments "
		"WHERE active = 1 "
		"ORDER BY id"));
 
	while (Stmt.Step() == ESQLitePreparedStatementStepResult::Row)
	{
		FOdysseyInvestment Inv;
		Stmt.GetColumnValueByName(TEXT("id"),          Inv.Id);
		Stmt.GetColumnValueByName(TEXT("name"),        Inv.Name);
		Stmt.GetColumnValueByName(TEXT("city"),        Inv.City);
		Stmt.GetColumnValueByName(TEXT("address"),     Inv.Address);
		Stmt.GetColumnValueByName(TEXT("description"), Inv.Description); // NULL -> puste
		Stmt.GetColumnValueByName(TEXT("currency"),    Inv.Currency);
		Result.Add(MoveTemp(Inv));
	}
 
	Stmt.Destroy();
	return Result;
}

TArray<FOdysseyBuilding> UOdysseyDatabaseSubsystem::GetBuildings(int32 InvestmentId)
{
	TArray<FOdysseyBuilding> Result;
	if (!IsOpen())
	{
		return Result;
	}
 
	// JOIN na investments, zeby nie pokazac budynkow ukrytej inwestycji
	FSQLitePreparedStatement Stmt;
	Stmt.Create(*Db, TEXT(
		"SELECT b.id, b.investment_id, b.name, b.code, b.floors_count, b.order_number "
		"FROM buildings b "
		"JOIN investments i ON i.id = b.investment_id "
		"WHERE b.investment_id = ?1 AND b.active = 1 AND i.active = 1 "
		"ORDER BY b.order_number, b.id"));
 
	Stmt.SetBindingValueByIndex(1, InvestmentId);
 
	while (Stmt.Step() == ESQLitePreparedStatementStepResult::Row)
	{
		FOdysseyBuilding B;
		Stmt.GetColumnValueByName(TEXT("id"),            B.Id);
		Stmt.GetColumnValueByName(TEXT("investment_id"), B.InvestmentId);
		Stmt.GetColumnValueByName(TEXT("name"),          B.Name);
		Stmt.GetColumnValueByName(TEXT("code"),          B.Code);
		Stmt.GetColumnValueByName(TEXT("floors_count"),  B.FloorsCount);
		Stmt.GetColumnValueByName(TEXT("order_number"),  B.OrderNumber);
		Result.Add(MoveTemp(B));
	}
 
	Stmt.Destroy();
	return Result;
}

TArray<FOdysseyFlat> UOdysseyDatabaseSubsystem::GetFlats(int32 BuildingId)
{
    TArray<FOdysseyFlat> Result;
    if (!IsOpen())
    {
        return Result;
    }
 
    // Kaskada widocznosci: flat + building + investment musza byc active = 1
    // Dzieki temu mieszkania z ukrytego Budynku E nie przejda, mimo ze same maja active = 1
    FSQLitePreparedStatement Stmt;
    Stmt.Create(*Db, TEXT(
        "SELECT f.id, f.building_id, f.status_id, f.name, f.entry_floor, f.floor, "
        "       f.num_rooms, f.area, f.price, f.price_sqm, f.mesh_id, f.description "
        "FROM flats f "
        "JOIN buildings b   ON b.id = f.building_id "
        "JOIN investments i ON i.id = b.investment_id "
        "WHERE f.building_id = ?1 "
        "  AND f.active = 1 AND b.active = 1 AND i.active = 1 "
        "ORDER BY f.entry_floor, f.name"));
 
    Stmt.SetBindingValueByIndex(1, BuildingId);
 
    while (Stmt.Step() == ESQLitePreparedStatementStepResult::Row)
    {
        FOdysseyFlat F;
        Stmt.GetColumnValueByName(TEXT("id"),          F.Id);
        Stmt.GetColumnValueByName(TEXT("building_id"), F.BuildingId);
        Stmt.GetColumnValueByName(TEXT("status_id"),   F.StatusId);
        Stmt.GetColumnValueByName(TEXT("name"),        F.Name);
        Stmt.GetColumnValueByName(TEXT("entry_floor"), F.EntryFloor);
        Stmt.GetColumnValueByName(TEXT("floor"),       F.Floor);
        Stmt.GetColumnValueByName(TEXT("num_rooms"),   F.NumRooms);
 
        double AreaTmp = 0.0;
        Stmt.GetColumnValueByName(TEXT("area"), AreaTmp);
        F.Area = static_cast<float>(AreaTmp);
    	
        ESQLiteColumnType PriceType = ESQLiteColumnType::Null;
        Stmt.GetColumnTypeByName(TEXT("price"), PriceType);
        if (PriceType != ESQLiteColumnType::Null)
        {
            F.bHasPrice = true;
            Stmt.GetColumnValueByName(TEXT("price"),     F.PriceGrosze);
            Stmt.GetColumnValueByName(TEXT("price_sqm"), F.PriceSqmGrosze);
        }
    	
        ESQLiteColumnType MeshType = ESQLiteColumnType::Null;
        Stmt.GetColumnTypeByName(TEXT("mesh_id"), MeshType);
        if (MeshType != ESQLiteColumnType::Null)
        {
            F.bHasMesh = true;
            Stmt.GetColumnValueByName(TEXT("mesh_id"), F.MeshId);
        }
 
        Stmt.GetColumnValueByName(TEXT("description"), F.Description); // NULL -> puste
 
        F.Extras = LoadExtrasForFlat(F.Id);
        Result.Add(MoveTemp(F));
    }
 
    Stmt.Destroy();
    return Result;
}

bool UOdysseyDatabaseSubsystem::GetStatus(int32 StatusId, FOdysseyFlatStatus& OutStatus)
{
	EnsureStatusesLoaded();
 
	if (const FOdysseyFlatStatus* Found = StatusCache.Find(StatusId))
	{
		OutStatus = *Found;
		return true;
	}
	
	return false;
}

TArray<FOdysseyFlatStatus> UOdysseyDatabaseSubsystem::GetAllStatuses()
{
	EnsureStatusesLoaded();
 
	TArray<FOdysseyFlatStatus> Out;
	StatusCache.GenerateValueArray(Out);
	return Out;
}

void UOdysseyDatabaseSubsystem::EnsureStatusesLoaded()
{
	if (bStatusesLoaded || !IsOpen())
	{
		return;
	}
 
	StatusCache.Reset();
 
	FSQLitePreparedStatement Stmt;
	Stmt.Create(*Db, TEXT("SELECT id, code, label_pl, color_hex FROM flat_statuses"));
 
	while (Stmt.Step() == ESQLitePreparedStatementStepResult::Row)
	{
		FOdysseyFlatStatus S;
		Stmt.GetColumnValueByName(TEXT("id"),        S.Id);
		Stmt.GetColumnValueByName(TEXT("code"),      S.Code);
		Stmt.GetColumnValueByName(TEXT("label_pl"),  S.Label);
		Stmt.GetColumnValueByName(TEXT("color_hex"), S.ColorHex);
		StatusCache.Add(S.Id, MoveTemp(S));
	}
 
	Stmt.Destroy();
	bStatusesLoaded = true;
}

TArray<FOdysseyExtra> UOdysseyDatabaseSubsystem::LoadExtrasForFlat(int32 FlatId)
{
	TArray<FOdysseyExtra> Extras;
	if (!IsOpen())
	{
		return Extras;
	}
 
	FSQLitePreparedStatement Stmt;
	Stmt.Create(*Db, TEXT(
		"SELECT e.id, e.name, fe.description "
		"FROM flat_flat_extra fe "
		"JOIN flat_extras e ON e.id = fe.flat_extra_id "
		"WHERE fe.flat_id = ?1 "
		"ORDER BY e.id"));
 
	Stmt.SetBindingValueByIndex(1, FlatId);
 
	while (Stmt.Step() == ESQLitePreparedStatementStepResult::Row)
	{
		FOdysseyExtra E;
		Stmt.GetColumnValueByName(TEXT("id"),          E.Id);
		Stmt.GetColumnValueByName(TEXT("name"),        E.Name);
		Stmt.GetColumnValueByName(TEXT("description"), E.Note); // NULL -> puste
		Extras.Add(MoveTemp(E));
	}
 
	Stmt.Destroy();
	return Extras;
}
