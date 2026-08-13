// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "OdysseyPawn.generated.h"

UCLASS(Blueprintable, BlueprintType)
class ODYSSEYCREW_API AOdysseyPawn : public APawn
{
	GENERATED_BODY()

public:

	AOdysseyPawn();

protected:

	virtual void BeginPlay() override;

public:	
	
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
