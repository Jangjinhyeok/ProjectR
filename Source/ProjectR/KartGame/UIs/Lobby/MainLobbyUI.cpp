#include "MainLobbyUI.h"

#include "EnumUtil.h"
#include "Components/Button.h"
#include "KartGame/Games/KartGameInstance.h"
#include "KartGame/Games/Modes/Lobby/LobbyPlayerController.h"
#include "SubUI/GameLobbySubUI.h"

void UMainLobbyUI::NativeConstruct()
{
	Super::NativeConstruct();

	GameLobbySubUI->GetGameSearchButton()->OnClicked.AddDynamic(this, &ThisClass::SearchGameAndJoinSessions);
	GameLobbySubUI->GetGameCreateButton()->OnClicked.AddDynamic(this, &ThisClass::CreateGameSessions);
}

void UMainLobbyUI::CreateGameSessions()
{
	GetGameInstance<UKartGameInstance>()->CreateNewGameSession();
}

void UMainLobbyUI::SearchGameAndJoinSessions()
{
	GetGameInstance<UKartGameInstance>()->SearchGameSession();
}

void UMainLobbyUI::SetDefaultWidgetInfo()
{
	IWidgetStackInterface::SetDefaultWidgetInfo();
	
	ALobbyPlayerController* PC = Cast<ALobbyPlayerController>(GetOwningPlayer());
	if (!PC)
	{
		return;
	}

	PC->OnClickInputKey_C_Notified.AddDynamic(this, &ThisClass::OpenGameModeLobby);
	PC->OnClickInputKey_F5_Notified.AddDynamic(this, &ThisClass::SearchGameAndJoinSessions);
}

void UMainLobbyUI::ClearWidgetInfo()
{
	IWidgetStackInterface::ClearWidgetInfo();
	
	ALobbyPlayerController* PC = Cast<ALobbyPlayerController>(GetOwningPlayer());
	if (!PC)
	{
		return;
	}
	
	PC->OnClickInputKey_C_Notified.RemoveDynamic(this, &ThisClass::OpenGameModeLobby);
	PC->OnClickInputKey_F5_Notified.RemoveDynamic(this, &ThisClass::SearchGameAndJoinSessions);
}

void UMainLobbyUI::OpenGameModeLobby()
{
	ALobbyPlayerController* PC = Cast<ALobbyPlayerController>(GetOwningPlayer());
	if (!PC)
	{
		return;
	}

	PC->PushWidgetStack(ELobbyUI::GameModeList);
}
