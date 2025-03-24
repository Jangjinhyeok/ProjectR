﻿#include "LobbyPlayerController.h"

#include "Blueprint/UserWidget.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "KartGame/UIs/Lobby/MainLobbyUI.h"
#include "KartGame/UIs/GameMode/GameModeListUI.h"
#include "EnumUtil.h"

void ALobbyPlayerController::BeginPlay()
{
	Super::BeginPlay();
	
	UEnhancedInputLocalPlayerSubsystem* SubSystem =
		ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
	if (SubSystem)
	{
		SubSystem->AddMappingContext(IMC_Lobby, 0);
	}
	
	SetShowMouseCursor(true);
	MainLobbyUI = CreateWidget<UMainLobbyUI>(this, MainLobbyUIClass);
	MainLobbyUI->AddToViewport();
	
	if (IWidgetStackInterface* WidgetStackInterface = Cast<IWidgetStackInterface>(MainLobbyUI))
	{
		WidgetStackInterface->SetDefaultWidgetInfo();
	}

	WidgetValue.Add(ELobbyUI::GameModeList, CreateWidget<UGameModeListUI>(this, GameModeListUIClass));
	WidgetValue[ELobbyUI::GameModeList]->SetVisibility(ESlateVisibility::Hidden);
	WidgetValue[ELobbyUI::GameModeList]->AddToViewport();
}

void ALobbyPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent);
	if (!EnhancedInputComponent)
	{
		return;
	}

	EnhancedInputComponent->BindAction(IA_ExitWidget, ETriggerEvent::Triggered, this,
		&ThisClass::PopWidgetStack);
	
	EnhancedInputComponent->BindAction(IA_C, ETriggerEvent::Triggered, this,
		&ThisClass::OnTrigger_C);
	EnhancedInputComponent->BindAction(IA_F5, ETriggerEvent::Triggered, this,
		&ThisClass::OnTrigger_F5);
}

void ALobbyPlayerController::PushWidgetStack(const ELobbyUI& LobbyUIKey)
{
	// 이전의 마지막 위젯에 대한 후처리
	// 위젯 스택이 비어있으면 로비이고, 아니라면 마지막 위젯 스택을 가져오게 처리한다.
	UUserWidget* LastWidget = WidgetStack.IsEmpty() ? MainLobbyUI : WidgetStack.Last();
	// 팝업 노출 시에는 뒤에 UI가 없어지면 안된다.
	LastWidget->SetVisibility(LobbyUIKey == ELobbyUI::Popup
		? ESlateVisibility::Visible : ESlateVisibility::Hidden);
	if (IWidgetStackInterface* WidgetStackInterface = Cast<IWidgetStackInterface>(LastWidget))
	{
		WidgetStackInterface->ClearWidgetInfo();
	}

	// 위젯 새로 넣고 처리하기
	UUserWidget* NewWidget = WidgetValue.FindRef(LobbyUIKey);
	WidgetStack.Push(NewWidget);
	NewWidget->SetVisibility(ESlateVisibility::Visible);
	if (IWidgetStackInterface* WidgetStackInterface = Cast<IWidgetStackInterface>(NewWidget))
	{
		WidgetStackInterface->SetDefaultWidgetInfo();
	}
}

void ALobbyPlayerController::PopWidgetStack()
{
	// 새로운 위젯이 들어오기 전의 처리
	if (WidgetStack.Num() == 0)
	{
		// TODO: ESC 화면 노출
		return;
	}

	WidgetStack.Last()->SetVisibility(ESlateVisibility::Hidden);
	if (IWidgetStackInterface* WidgetStackInterface = Cast<IWidgetStackInterface>(WidgetStack.Last()))
	{
		WidgetStackInterface->ClearWidgetInfo();
	}
	
	WidgetStack.Pop();

	// 위젯 빠진 이후에 대한 처리
	if (WidgetStack.Num() == 0)
	{
		MainLobbyUI->SetVisibility(ESlateVisibility::Visible);
		if (IWidgetStackInterface* WidgetStackInterface = Cast<IWidgetStackInterface>(MainLobbyUI))
		{
			WidgetStackInterface->SetDefaultWidgetInfo();
		}
		return;
	}
	
	if (IWidgetStackInterface* WidgetStackInterface = Cast<IWidgetStackInterface>(WidgetStack.Last()))
	{
		WidgetStackInterface->SetDefaultWidgetInfo();
	}
}

void ALobbyPlayerController::OnTrigger_C()
{
	OnClickInputKey_C_Notified.Broadcast();
}

void ALobbyPlayerController::OnTrigger_F5()
{
	OnClickInputKey_F5_Notified.Broadcast();
}
