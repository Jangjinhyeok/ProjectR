#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CheckPoint.generated.h"

class UBoxComponent;

UCLASS()
class PROJECTR_API ACheckPoint : public AActor
{
	GENERATED_BODY()

public:
	ACheckPoint();

	FORCEINLINE FString GetCurrentCheckPoint() const { return CurrentCheckPoint; }
	FORCEINLINE FString GetNextCheckPoint() const { return NextCheckPoint; }

	uint16 GetPinMainNumber() const;
	
protected:
	virtual void BeginPlay() override;
	
private:
	TArray<uint16> CurrentCheckPointPinNumArray;
	
	UPROPERTY(EditAnywhere, Category = "Options|CheckPoint", meta = (AllowPrivateAccess = true))
	FString CurrentCheckPoint;
	
	UPROPERTY(EditAnywhere, Category = "Options|CheckPoint", meta = (AllowPrivateAccess = true))
	FString NextCheckPoint;
	
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	TObjectPtr<UBoxComponent> CheckBox;

	UFUNCTION()
	void OnMoveToCheckPoint(UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
	void SaveCheckPoint(const AActor* CheckKart);
};
