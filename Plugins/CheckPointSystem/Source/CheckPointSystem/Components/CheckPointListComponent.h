#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CheckPointListComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class CHECKPOINTSYSTEM_API UCheckPointListComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UCheckPointListComponent();

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditInstanceOnly, Category = "Options")
	TArray<AActor*> CheckPoints;
};