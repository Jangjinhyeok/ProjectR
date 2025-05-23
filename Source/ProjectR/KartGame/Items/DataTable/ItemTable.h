﻿// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "ItemTable.generated.h"

/**
 * 
 */
UENUM(BlueprintType)
enum class EItemName : uint8
{
	None,
	Booster,
	Missile,
	WaterBomb,
	Shield,
	ItemBox,
};
USTRUCT(BlueprintType)
struct PROJECTR_API FItemTable : public FTableRowBase
{
	GENERATED_BODY()
	
public:
	FItemTable()
	{
		ItemName = EItemName::None;
		ItemWeight = 0;
		ItemType = false;
		ItemClass = nullptr;
		ItemImage = nullptr;
	};
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Item)
	int32 ItemID = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Item)
	EItemName ItemName;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Item)
	int32 ItemWeight;

	// true = 조준형 , false = 즉발형
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Item)
	bool ItemType; 
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Item)
	TSubclassOf<AActor> ItemClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Item)
	class UTexture2D* ItemImage;
};
