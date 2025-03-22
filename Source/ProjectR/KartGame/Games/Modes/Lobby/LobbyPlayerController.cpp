#include "LobbyPlayerController.h"

#include "Blueprint/UserWidget.h"
#include "EnhancedInputComponent.h"
#include "KartGame/UIs/Lobby/MainLobbyUI.h"

void ALobbyPlayerController::BeginPlay()
{
	Super::BeginPlay();

	SetShowMouseCursor(true);
	MainLobbyUI = CreateWidget<UMainLobbyUI>(this, MainLobbyUIClass);
	MainLobbyUI->AddToViewport(0);
}

void ALobbyPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent);
	if (!EnhancedInputComponent)
	{
		return;
	}

	EnhancedInputComponent->BindAction(IA_ExitWidget, ETriggerEvent::Started, this,
		&ThisClass::PopWidgetStack);
}

void ALobbyPlayerController::PushWidgetStack(UUserWidget* NewWidget)
{
	WidgetStack.Push(NewWidget);
	
	WidgetStack.Last()->SetVisibility(ESlateVisibility::Hidden);
	NewWidget->SetVisibility(ESlateVisibility::Collapsed);
}

void ALobbyPlayerController::PopWidgetStack()
{
	if (WidgetStack.Num() == 0)
	{
		// TODO: ESC 화면 노출
		return;
	}

	WidgetStack.Last()->SetVisibility(ESlateVisibility::Hidden);
	WidgetStack.Pop();
}