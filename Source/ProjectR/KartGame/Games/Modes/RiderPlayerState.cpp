﻿#include "RiderPlayerState.h"

#include "Net/UnrealNetwork.h"
#include "RaceGameState.h"
#include "ProjectR/KartGame/Games/Objects/CheckPoint.h"
#include "ProjectR/KartGame/Utils/CheckPointUtil.h"

void ARiderPlayerState::BeginPlay()
{
	Super::BeginPlay();
	
	FCheckPointUtil::GetCheckPointPinInfo(CurrentKartCheckPoint,
		PlayerCurrentCheckPointPinList);
}

uint16 ARiderPlayerState::GetCurrentMainCheckPoint() const
{
	return PlayerCurrentCheckPointPinList[0];
}

TObjectPtr<ACheckPoint> ARiderPlayerState::GetNextNearCheckPoint() const
{
	const ACheckPoint* CurrentCheckPointPin = GetWorld()->GetGameState<ARaceGameState>()
				->GetCheckPointData().FindRef(CurrentKartCheckPoint);
	return GetWorld()->GetGameState<ARaceGameState>()
				->GetCheckPointData().FindRef(CurrentCheckPointPin->GetNextCheckPoint());
}

void ARiderPlayerState::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ARiderPlayerState, CurrentLap);
	DOREPLIFETIME(ARiderPlayerState, CurrentKartCheckPoint);
}