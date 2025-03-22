#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameStartUI.generated.h"

class ULicenseUI;
class UGameStartCinematicUI;
class UWidgetSwitcher;

UCLASS()
class PROJECTR_API UGameStartUI : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintImplementableEvent)
	void EndSubUI();
	
protected:
	virtual void NativeConstruct() override;
	
private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UWidgetSwitcher> StartSubUISwitcher;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UGameStartCinematicUI> StartCinematicUI;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<ULicenseUI> LicenseUI;

	UFUNCTION()
	void GoNextSubUI();
};
