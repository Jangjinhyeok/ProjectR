#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameModeListUI.generated.h"

class UWidgetSwitcher;

UCLASS()
class PROJECTR_API UGameModeListUI : public UUserWidget
{
	GENERATED_BODY()
	
private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UWidgetSwitcher> GameModeSwitcher;
};
