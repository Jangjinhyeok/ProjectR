// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "WidgetStackInterface.generated.h"

UINTERFACE()
class UWidgetStackInterface : public UInterface
{
	GENERATED_BODY()
};

class PROJECTR_API IWidgetStackInterface
{
	GENERATED_BODY()

public:
	UFUNCTION()
	virtual void SetDefaultWidgetInfo() {}
	UFUNCTION()
	virtual void ClearWidgetInfo() {}
};
