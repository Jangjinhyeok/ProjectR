﻿#include "RiderPlayerState.h"

#include "Net/UnrealNetwork.h"
#include "RaceGameState.h"
#include "ProjectR/KartGame/Games/Objects/CheckPoint.h"
#include "ProjectR/KartGame/Utils/CheckPointUtil.h"

ARiderPlayerState::ARiderPlayerState()
{
	SetReplicates(true);
}

void ARiderPlayerState::BeginPlay()
{
	Super::BeginPlay();
	
	FCheckPointUtil::GetCheckPointPinInfo(CurrentKartCheckPoint,
		PlayerCurrentCheckPointPinList);
}

void ARiderPlayerState::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ARiderPlayerState, CurrentLap);
	DOREPLIFETIME(ARiderPlayerState, CurrentKartCheckPoint);
	DOREPLIFETIME(ARiderPlayerState, Ranking);
}

void ARiderPlayerState::SetCheckPoint(const FString& CheckPointNum)
{
	CurrentKartCheckPoint = CheckPointNum;
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

	if (!IsValid(CurrentCheckPointPin))
	{
		return nullptr;
	}
	
	return GetWorld()->GetGameState<ARaceGameState>()
				->GetCheckPointData().FindRef(CurrentCheckPointPin->GetNextCheckPoint());
}

void ARiderPlayerState::GoNextLap()
{
	CurrentLap += 1;
	CurrentKartCheckPoint = TEXT("0");

	if (HasAuthority())
	{
		if (!GetPawn()->IsLocallyControlled())
		{
			return;
		}
		OnGoNextLapNotified.Broadcast(CurrentLap);
	}
}

void ARiderPlayerState::OnRep_CurrentLap()
{
	OnGoNextLapNotified.Broadcast(CurrentLap);
}
