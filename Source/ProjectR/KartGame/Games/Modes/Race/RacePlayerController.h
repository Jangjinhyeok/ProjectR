﻿#pragma once

#include "CoreMinimal.h"
#include "CommonUtil.h"
#include "GameFramework/PlayerController.h"
#include "RacePlayerController.generated.h"

class AKart;
class UTrackLoadingUI;
class UMainUI;

UCLASS()
class PROJECTR_API ARacePlayerController : public APlayerController
{
	GENERATED_BODY()

public:
#pragma region GetterSetter
	GETTER(TObjectPtr<UMainUI>, MainHUD);
	
#pragma endregion
	void SetHUDToStart();

	UFUNCTION(Client, Reliable)
	void Client_SetHUDToStart();

	void CountDownToEndGame();

	UFUNCTION(Client, Reliable)
	void Client_CountDownToEndGame();

	void SpawnKartWithCheckPoint(const uint8 Index);
	
private:
#pragma region UIFactory
	UPROPERTY(EditDefaultsOnly, Category = "Options|UI", meta = (AllowPrivateAccess = true))
	TSubclassOf<UMainUI> MainHUDClass;
#pragma endregion
	
	UPROPERTY()
	TObjectPtr<UMainUI> MainHUD;

	UPROPERTY()
	TSubclassOf<AKart> KartClass;

	UFUNCTION()
	void KartSetToMove();

	UFUNCTION()
	void EndGame();
};