﻿// Fill out your copyright notice in the Description page of Project Settings.


#include "ItemInteractionComponent.h"

#include "FastLogger.h"
#include "Kart.h"
#include "KartAccelerationComponent.h"
#include "Components/BoxComponent.h"
#include "KartGame/Games/Modes/Race/RacePlayerController.h"
#include "KartGame/UIs/HUD/MainUI.h"
#include "KartGame/UIs/NotificationTextUI/NotificationTextUI.h"
#include "Net/UnrealNetwork.h"


UItemInteractionComponent::UItemInteractionComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	bWantsInitializeComponent = true;
	SetIsReplicatedByDefault(true);

}

void UItemInteractionComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UItemInteractionComponent::InitializeComponent()
{
	Super::InitializeComponent();
	
	Kart = Cast<AKart>(GetOwner());
}

void UItemInteractionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (bIsInteraction)
	{
		switch (CurrentType)
		{
		case EInteractionType::Explosion:
			{
				MissileInteraction_Move(DeltaTime);
				break;
			}
		case EInteractionType::Water:
			{
				WaterBombInteraction_Move(DeltaTime);
				break;
			}
		default:
			{
				break;	
			}
		}
	}

	if (bShieldOn)
	{
		Server_CheckShieldUsingTime();
	}
}

void UItemInteractionComponent::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UItemInteractionComponent, bIsInteraction);
}

void UItemInteractionComponent::Interaction(EInteractionType interactionType)
{
	if (Kart == nullptr)
	{
		FFastLogger::LogConsole(TEXT("InteractionComponent : Kart is nullptr"));
		return;
	}

	if (bShieldOn)
	{
		FFastLogger::LogConsole(TEXT("InteractionComponent : blocked by shield"));
		NetMulticast_ShieldEffect(false);
		if (Kart->HasAuthority())
		{
			bShieldOn = false;
		}
		ShieldElapsedTime = 0.f;
		return;
	}

	bIsInteraction = true;
	CurrentType = interactionType;

	switch (CurrentType)
	{
	case EInteractionType::Explosion:
		{
			MissileHitInteraction();
			break;
		}
	case EInteractionType::Water:
		{
			WaterBombHitInteraction();
			break;
		}
	default:
		{
			break;	
		}
	}
	
}

void UItemInteractionComponent::MissileHitInteraction()
{
	InitialPos = Kart->GetActorLocation();
	InitialQuat = Kart->GetActorQuat();
	Client_ChangePhysics(false);
}

void UItemInteractionComponent::MissileInteraction_Move(float DeltaTime)
{
	if (Kart->HasAuthority() == false) return;

	MissileKnockbackElapsedTime += DeltaTime;
	float alpha = (MissileKnockbackElapsedTime / MissileKnockbackTime);

	// 회전계산
	// ease-out 곡선 적용 (처음엔 빠르게 점점 천천히)
	float easedAlpha = FMath::Sin(alpha * PI * 0.5f);
	float rotationAngle = 360.f * MissileKnockbackRotationNumber * easedAlpha;
		
	// 시작 회전에서 현재까지의 회전 계산
	FQuat rotationQuat = FQuat(FRotator(rotationAngle, 0, 0));
	FQuat resultQuat = rotationQuat * InitialQuat;

	// 위치계산
	float newZ = FMath::Lerp(InitialPos.Z, InitialPos.Z + MissileKnockbackHeight, easedAlpha);
		
	FVector resultPos = FVector(InitialPos.X, InitialPos.Y, newZ);
	
	if (MissileKnockbackElapsedTime >= MissileKnockbackTime)
	{
		bIsInteraction = false;
		CurrentType = EInteractionType::None;
		MissileKnockbackElapsedTime = 0.f;
		Client_ChangePhysics(true);
		return;
	}
	NetMulticast_MissileInteraction_Move(resultQuat, resultPos);
}

void UItemInteractionComponent::WaterBombHitInteraction()
{
	InitialPos = Kart->GetActorLocation();
	InitialQuat = Kart->GetActorQuat();
	InitialRot = Kart->GetActorRotation();
	Client_ChangePhysics(false);
}

void UItemInteractionComponent::WaterBombInteraction_Move(float DeltaTime)
{
	if (Kart->HasAuthority() == false) return;

	WaterBombInteractionElapsedTime += DeltaTime;

	// 1초안에 공중으로 뜬다
	// Ease-out 곡선을 통해 처음엔 빠르게 이후엔 천천히 올라감 (1초라 티가 안남)
	float alpha = WaterBombInteractionElapsedTime / 1.0f;
	float easedAlpha = FMath::Sin(alpha * PI * 0.5f);
	float newZ = FMath::Lerp(InitialPos.Z, InitialPos.Z + WaterBombInteractionHeight, easedAlpha);
	FVector resultPos;
	if (WaterBombInteractionElapsedTime < 1.0f)
	{
		resultPos = FVector(InitialPos.X, InitialPos.Y, newZ);
	}
	else
	{
		resultPos = Kart->GetActorLocation();
	}

	float RollOffset = FMath::Sin(WaterBombInteractionElapsedTime * RotateSpeed) * MaxRoll;
	float PitchOffset = FMath::Sin(WaterBombInteractionElapsedTime * RotateSpeed * 1.2f) * MaxPitch;
	FRotator resultRot = InitialRot + FRotator(PitchOffset, 0.f, RollOffset);

	if (WaterBombInteractionElapsedTime >= WaterBombInteractionTime)
	{
		bIsInteraction = false;
		CurrentType = EInteractionType::None;
		WaterBombInteractionElapsedTime = 0.f;
		resultRot = InitialRot;	
		Client_ChangePhysics(true);
	}
	
	NetMulticast_WaterBombInteraction_Move(resultPos, resultRot);
}

void UItemInteractionComponent::NetMulticast_WaterBombInteraction_Move_Implementation(FVector resultPos, FRotator resultRot)
{
	Kart->SetActorLocation(resultPos);
	Kart->SetActorRotation(resultRot);
}

void UItemInteractionComponent::NetMulticast_MissileInteraction_Move_Implementation(FQuat resultQuat, FVector resultPos)
{
	Kart->SetActorRotation(resultQuat);
	Kart->SetActorLocation(resultPos);
}

void UItemInteractionComponent::Client_ChangePhysics_Implementation(bool bEnable)
{
	if (Kart)
	{
		if (GetOwnerRole() == ROLE_Authority)
		{
			return;
		}
		Kart->GetRootBox()->SetSimulatePhysics(bEnable);
		Kart->GetAccelerationComponent()->ResetAcceleration();
	}
}

void UItemInteractionComponent::Server_CheckShieldUsingTime_Implementation()
{
	NetMulticast_ShieldEffect(true);
	ShieldElapsedTime += GetWorld()->GetDeltaSeconds();
	if (ShieldElapsedTime >= ShieldTime)
	{
		NetMulticast_ShieldEffect(false);
		bShieldOn = false;
		ShieldElapsedTime = 0.f;
	}
}

void UItemInteractionComponent::NetMulticast_ShieldEffect_Implementation(bool value)
{
	if (Kart->IsLocallyControlled() == false) return;
	
	auto* pc = Cast<ARacePlayerController>(GetWorld()->GetFirstPlayerController());
	if (pc)
	{
		pc->GetMainHUD()->GetWBP_NotificationTextUI()->SetShieldTextVisible(value);
	}
}

