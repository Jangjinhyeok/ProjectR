#include "SessionGameState.h"
#include "GameFramework/PlayerState.h"
#include "SessionPlayerController.h"

ASessionGameState::ASessionGameState()
{
	PlayerInfo.Init(TEXT(""), MaxPlayerCount);
	for (int i = 0; i < MaxPlayerCount; i++)
	{
		PlayerInfoMap.Add(i , TEXT(""));
	}
}

void ASessionGameState::BeginPlay()
{
	Super::BeginPlay();
}

void ASessionGameState::JoinPlayer(APlayerController* PlayerController)
{
	const FString PlayerName = PlayerController->GetPlayerState<APlayerState>()->GetPlayerName();
	
	ReadyMap.Add(PlayerName, false);

	for (int i = 0; i < MaxPlayerCount; i++)
	{
		if (PlayerInfo[i] == TEXT(""))
		{
			PlayerInfo[i] = PlayerName;
			PlayerInfoMap[i] = PlayerName;
			break;
		}
	}
	
	UpdateSessionInfo();
}

void ASessionGameState::LeavePlayer(APlayerController* PlayerController)
{
	const FString PlayerName = PlayerController->GetPlayerState<APlayerState>()->GetPlayerName();
	
	const uint8 PlayerIndex = *PlayerInfoMap.FindKey(PlayerName);
	PlayerInfoMap.Remove(PlayerIndex);
	PlayerInfo[PlayerIndex] = TEXT("");
	
	ReadyMap.Remove(PlayerName);
	
	UpdateSessionInfo();
}

void ASessionGameState::UpdateSessionInfo()
{
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		ASessionPlayerController* PC = Cast<ASessionPlayerController>(*It);
		if (PC->IsLocalController())
		{
			PC->UpdateSessionList(PlayerInfo);
		} else
		{
			PC->Client_UpdatePlayerInfo(PlayerInfo);
		}
	}
}

bool ASessionGameState::IsPlayerReadyAll()
{
	for (const TTuple<FString, bool> Map : ReadyMap)
	{
		if (!Map.Value)
		{
			return false;
		}
	}

	return true;
}