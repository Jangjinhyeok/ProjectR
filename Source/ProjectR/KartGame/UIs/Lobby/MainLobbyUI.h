#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "KartGame/UIs/_Common/WidgetStackInterface.h"
#include "MainLobbyUI.generated.h"

class UInputAction;
class UGameLobbySubUI;
class UWidgetSwitcher;

UCLASS()
class PROJECTR_API UMainLobbyUI : public UUserWidget, public IWidgetStackInterface
{
	GENERATED_BODY()

public:
	virtual void SetDefaultWidgetInfo() override;
	virtual void ClearWidgetInfo() override;
	
protected:
	virtual void NativeConstruct() override;
	
private: 
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UWidgetSwitcher> LobbyUISwitcher;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UGameLobbySubUI> GameLobbySubUI;

	UFUNCTION()
	void CreateGameSessions();
	
	UFUNCTION()
	void SearchGameAndJoinSessions();

	UFUNCTION()
	void OpenGameModeLobby();
};
