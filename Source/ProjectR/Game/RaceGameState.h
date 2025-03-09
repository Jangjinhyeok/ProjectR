﻿#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "RaceGameState.generated.h"

UCLASS()
class PROJECTR_API ARaceGameState : public AGameStateBase
{
	GENERATED_BODY()
	
public:
	FORCEINLINE uint16 GetMaxCheckPoint() const { return MaxCheckPoint; }
	FORCEINLINE uint16 GetMaxLaps() const { return MaxLaps; }

protected:
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaSeconds) override;
	
private:
	UPROPERTY(EditDefaultsOnly, Category = "Options|Race", meta = (AllowPrivateAccess = true))
	uint8 MaxLaps = 1;
	
	uint16 MaxCheckPoint = 0;
};