#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "LobbyPlayerController.generated.h"

class UInputAction;
class UInputMappingContext;
class UMainLobbyUI;

UCLASS()
class PROJECTR_API ALobbyPlayerController : public APlayerController
{
	GENERATED_BODY()
	
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
	UPROPERTY(EditDefaultsOnly, Category = "Options|Input", meta = (AllowPrivateAccess = true))
	TObjectPtr<UInputMappingContext> IMC_Lobby;
	
	UPROPERTY(EditDefaultsOnly, Category = "Options|Input", meta = (AllowPrivateAccess = true))
	TObjectPtr<UInputAction> IA_ExitWidget;
#pragma endregion

#pragma region WidgetFactory
	UPROPERTY(EditDefaultsOnly, Category = "Options|UI", meta = (AllowPrivateAccess = true))
	TSubclassOf<UMainLobbyUI> MainLobbyUIClass;
#pragma endregion 
};
