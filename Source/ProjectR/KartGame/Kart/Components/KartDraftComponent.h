﻿// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "KartDraftComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PROJECTR_API UKartDraftComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UKartDraftComponent();

protected:
	virtual void BeginPlay() override;

	virtual void InitializeComponent() override;

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
	UFUNCTION(NetMulticast, Reliable)
	void DrawTraceLineBox(FVector start, FVector end, FVector boxHalfSize, FColor boxColor);

	UFUNCTION(Server, Reliable)
	void Server_FindTarget(FVector start, FVector end, FVector boxHalfSize);

	UFUNCTION(Server, Reliable)
	void Server_CheckTraceTime();

	void AddDraftForce();

	UFUNCTION(NetMulticast, Reliable)
	void NetMulticast_DraftEffect(bool value);
	
private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Draft", meta = (AllowPrivateAccess = "true"))
	class AKart* Kart = nullptr;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Draft", meta = (AllowPrivateAccess = "true"))
	class AKart* FinalTarget = nullptr;

	UPROPERTY(Replicated)
	float ElapsedTime = 0.f;

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Draft", meta = (AllowPrivateAccess = "true"))
	bool bDraftStart = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Draft", meta = (AllowPrivateAccess = "true"))
	float DraftStartTime = 3.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Draft", meta = (AllowPrivateAccess = "true"))
	float DraftDuration = 1.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Draft", meta = (AllowPrivateAccess = "true"))
	float DraftDistance = 1000.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Draft", meta = (AllowPrivateAccess = "true"))
	FVector TraceBoxHalfSize = FVector(100.f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Draft", meta = (AllowPrivateAccess = "true"))
	float DraftForce = 500.f;

	UPROPERTY()
	FTimerHandle DraftTimerHandle;

	UPROPERTY()
	TArray<AActor*> IgnoredActors;

	
};
