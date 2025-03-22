#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "LobbyPlayerController.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnClickInputKey_C_Notified);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnClickInputKey_F5_Notified);

enum class ELobbyUI : uint8;

class UGameModeListUI;
class UInputAction;
class UInputMappingContext;
class UMainLobbyUI;

UCLASS()
class PROJECTR_API ALobbyPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	FOnClickInputKey_C_Notified OnClickInputKey_C_Notified;
	FOnClickInputKey_F5_Notified OnClickInputKey_F5_Notified;
	
protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;
	
private:
	UPROPERTY()
	TObjectPtr<UMainLobbyUI> MainLobbyUI;

	// TODO: 모듈화
	TArray<TObjectPtr<UUserWidget>> WidgetStack;

	void PushWidgetStack(UUserWidget* NewWidget);
	void PopWidgetStack();

#pragma region Input
	UPROPERTY(EditDefaultsOnly, Category = "Options|Input|Key", meta = (AllowPrivateAccess = true))
	TObjectPtr<UInputAction> IA_C;
	UPROPERTY(EditDefaultsOnly, Category = "Options|Input|Key", meta = (AllowPrivateAccess = true))
	TObjectPtr<UInputAction> IA_F5;

	void OnTrigger_C();
	void OnTrigger_F5();
	
	UPROPERTY(EditDefaultsOnly, Category = "Options|Input", meta = (AllowPrivateAccess = true))
	TObjectPtr<UInputMappingContext> IMC_Lobby;
	
	UPROPERTY(EditDefaultsOnly, Category = "Options|Input", meta = (AllowPrivateAccess = true))
	TObjectPtr<UInputAction> IA_ExitWidget;
#pragma endregion

#pragma region WidgetInfo
	UPROPERTY()
	TMap<ELobbyUI, TObjectPtr<UUserWidget>> WidgetValue;
	
	UPROPERTY(EditDefaultsOnly, Category = "Options|UI", meta = (AllowPrivateAccess = true))
	TSubclassOf<UMainLobbyUI> MainLobbyUIClass;
	
	UPROPERTY(EditDefaultsOnly, Category = "Options|UI", meta = (AllowPrivateAccess = true))
	TSubclassOf<UGameModeListUI> GameModeListUIClass;
#pragma endregion 
};
