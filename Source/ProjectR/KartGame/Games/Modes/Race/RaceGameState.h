﻿#pragma once

#include "CoreMinimal.h"
#include "CommonUtil.h"
#include "EnumUtil.h"
#include "GameFramework/GameStateBase.h"
#include "RaceGameState.generated.h"

enum class ERaceStatus;
class ACheckPoint;


UCLASS()
class PROJECTR_API ARaceGameState : public AGameStateBase
{
	GENERATED_BODY()
	
public:
	ARaceGameState();
	FORCEINLINE uint16 GetMaxCheckPoint() const { return MaxCheckPoint; }
	FORCEINLINE uint16 GetMaxLaps() const { return MaxLaps; }
	FORCEINLINE TMap<FString, ACheckPoint*> GetCheckPointData() const { return CheckPointData; }

#pragma region GetSet
	GETTER_SETTER(ERaceStatus, RaceStatus)
	GETTER_SETTER(FDateTime, RaceStartTime)
#pragma endregion
	
protected:
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaSeconds) override;
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
private:
	UPROPERTY(EditDefaultsOnly, Category = "Options|Race", meta = (AllowPrivateAccess = true))
	uint8 MaxLaps = 3;
	
	uint16 MaxCheckPoint = 0;

	UPROPERTY()
	TMap<FString, ACheckPoint*> CheckPointData;

	void SortRank();

	UPROPERTY(Replicated)
	ERaceStatus RaceStatus = ERaceStatus::Idle;

	UPROPERTY(Replicated)
	FDateTime RaceStartTime;
};