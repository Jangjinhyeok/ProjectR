﻿#include "RaceGameMode.h"

#include "SessionUtil.h"
#include "OnlineSessionSettings.h"
#include "RaceGameState.h"
#include "RacePlayerController.h"
#include "GameFramework/PlayerState.h"

void ARaceGameMode::BeginPlay()
{
	Super::BeginPlay();
}

void ARaceGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	if (!FSessionUtil::GetCurrentSession())
	{
		GetWorld()->GetTimerManager().SetTimer(GameStartTimerHandle, this,
			&ThisClass::StartGame, 0.5, false);
		return;
	}

	StartToPlayerCount += 1;
	ARacePlayerController* PC = Cast<ARacePlayerController>(NewPlayer);
	
	if (PC->HasAuthority())
	{
		PC->SpawnKartWithCheckPoint(StartToPlayerCount);
	}
	
	const uint8 MaxPlayerCount = FSessionUtil::GetCurrentSession()->SessionSettings.NumPublicConnections;
	const uint8 RemainPlayerCount = FSessionUtil::GetCurrentSession()->NumOpenPublicConnections;
	
	if (StartToPlayerCount == (MaxPlayerCount - RemainPlayerCount))
	{
		GetWorld()->GetTimerManager().SetTimer(GameStartTimerHandle, this,
			&ThisClass::StartGame, 3, false);
	}
}

void ARaceGameMode::StartGame()
{
	/**
	 * TODO: 여기에 게임 시작을 위한 스폰 위치 조정 작업과
	 * 게임 시작을 위한 트리거 설정을 진행한다.
	 */
	ARaceGameState* GS = GetGameState<ARaceGameState>();
	
	if (GS->GetRaceStatus() != ERaceStatus::Idle)
	{
		return;
	}
	
	GS->SetRaceStatus(ERaceStatus::Ready);

	for (int i = 0; i < GS->PlayerArray.Num(); i++)
	{
		ARacePlayerController* PC = Cast<ARacePlayerController>(
			GS->PlayerArray[i]->GetPlayerController());
		
		if (!PC)
		{
			return;
		}

		if (PC->IsLocalController())
		{
			PC->SetHUDToStart();
		} else
		{
			PC->Client_SetHUDToStart();
		}
	}
}