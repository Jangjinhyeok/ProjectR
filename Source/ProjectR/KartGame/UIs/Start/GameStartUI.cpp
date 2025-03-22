#include "GameStartUI.h"

#include "Components/WidgetSwitcher.h"
#include "SubUI/GameStartCinematicUI.h"
#include "SubUI/LicenseUI.h"

void UGameStartUI::NativeConstruct()
{
	Super::NativeConstruct();

	StartCinematicUI->OnCinematicEndNotified.Clear();
	StartCinematicUI->OnCinematicEndNotified.AddDynamic(this, &ThisClass::GoNextSubUI);
	LicenseUI->OnLicenseEndNotified.AddDynamic(this, &ThisClass::EndSubUI);
}

void UGameStartUI::GoNextSubUI()
{
	StartSubUISwitcher->SetActiveWidgetIndex(StartSubUISwitcher->GetActiveWidgetIndex() + 1);
}