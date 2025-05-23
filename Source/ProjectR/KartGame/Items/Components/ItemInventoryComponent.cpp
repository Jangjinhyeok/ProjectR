﻿// Fill out your copyright notice in the Description page of Project Settings.


#include "ItemInventoryComponent.h"

#include "EngineUtils.h"
#include "EnhancedInputComponent.h"
#include "FastLogger.h"
#include "ItemInteractionComponent.h"
#include "Kart.h"
#include "KartAccelerationComponent.h"
#include "KartFrictionComponent.h"
#include "KartItemSoundComponent.h"
#include "KartSteeringComponent.h"
#include "Components/Image.h"
#include "Components/WidgetComponent.h"
#include "KartGame/Games/Modes/Race/RacePlayerController.h"
#include "KartGame/Items/Booster/Booster.h"
#include "KartGame/Items/Missile/Missile.h"
#include "KartGame/Items/Shield/Shield.h"
#include "KartGame/Items/WaterBomb/WaterBomb.h"
#include "KartGame/UIs/HUD/MainUI.h"
#include "KartGame/UIs/HUD/Aim/Aim.h"
#include "KartGame/UIs/HUD/ItemInventory/ItemInventory.h"
#include "Net/UnrealNetwork.h"


UItemInventoryComponent::UItemInventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	bWantsInitializeComponent = true;
	SetIsReplicatedByDefault(true);
	
	ConstructorHelpers::FObjectFinder<UInputAction> ia_useitem(TEXT("'/Game/Kart/Input/InputAction/IA_UseItem.IA_UseItem'"));
	if (ia_useitem.Succeeded())
	{
		IA_UseItem = ia_useitem.Object;
	}

	
	Items = LoadObject<UDataTable>(nullptr, TEXT("'/Game/Items/DataTable/ItemTable.ItemTable'"));
	if (Items)
	{
		TArray<FName> RowNames = Items->GetRowNames();

		for (const FName& RowName : RowNames)
		{
			FItemTable* Row = Items->FindRow<FItemTable>(RowName, TEXT(""));
			if (Row != nullptr)
			{
				ItemMap.Add(Row->ItemID, *Row);
			}
		}
	}
}

void UItemInventoryComponent::BeginPlay()
{
	Super::BeginPlay();
	InitialAimUIPos = Kart->GetUsingAimComponent()->GetRelativeLocation();
	InitialAimUIScale = Kart->GetUsingAimComponent()->GetRelativeScale3D();

	for (TActorIterator<AActor> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		AActor* actor = *ActorItr;
		if (Cast<AKart>(actor))
		{
			continue;
		}
		IgnoredActors.Add(*ActorItr);
	}

}

void UItemInventoryComponent::InitializeComponent()
{
	Super::InitializeComponent();

	Kart = Cast<AKart>(GetOwner());
	if (Kart)
	{
		Kart->OnInputBindingDelegate.AddDynamic(this, &UItemInventoryComponent::SetupInputBinding);
		Kart->GetFrictionComponent()->OnBoosterMade.AddDynamic(this, &UItemInventoryComponent::GetBoosterItem);
	}
}

void UItemInventoryComponent::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UItemInventoryComponent, bInventoryIsFull);
	DOREPLIFETIME(UItemInventoryComponent, Inventory);
	DOREPLIFETIME(UItemInventoryComponent, LockedTarget);
}

void UItemInventoryComponent::SetupInputBinding(class UEnhancedInputComponent* PlayerInputComponent)
{
	PlayerInputComponent->BindAction(IA_UseItem, ETriggerEvent::Ongoing, this, &UItemInventoryComponent::LockPlayer);
	PlayerInputComponent->BindAction(IA_UseItem, ETriggerEvent::Completed, this, &UItemInventoryComponent::UseItem);
}

void UItemInventoryComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UItemInventoryComponent::GetItem(const FItemTable itemData)
{
	if (bInventoryIsFull)
	{
		return;
	}
	
	Server_GetItem(itemData);
}

void UItemInventoryComponent::Server_GetItem_Implementation(const FItemTable itemData)
{
	NetMulticast_GetItem(itemData);
}

void UItemInventoryComponent::NetMulticast_GetItem_Implementation(const FItemTable itemData)
{
	Inventory.Add(itemData);
	
	ARacePlayerController* pc = Cast<ARacePlayerController>(Kart->GetController());
	if (pc)
	{
		UMainUI* mainUI = pc->GetMainHUD();
		if (mainUI)
		{
			UItemInventory* inventoryUI = mainUI->GetWBP_ItemInventory();
			if (inventoryUI)
			{
				inventoryUI->GetItemUI(itemData.ItemImage);
			}
		}
	}
	
	if (Inventory.Num() == MaxInventorySpace)
	{
		bInventoryIsFull = true;
	}
}

void UItemInventoryComponent::GetBoosterItem()
{
	if (GetWorld()->GetMapName().Contains(TEXT("ItemTrack"))) return;
	
	FFastLogger::LogConsole(TEXT("부스터 얻기"));
	GetItem(ItemMap[1]);
}

void UItemInventoryComponent::UseItem()
{
	// 마지막 랩 도착 후 안움직일 때 아이템 사용 방지
	if (!Kart->GetbCanMove())
	{
		return;
	}

	// 아이템 없을 때 아이템 사용 방지
	if (Inventory.Num() == 0)
	{
		return;
	}
	
	// 부스터 사용중에 다른 부스터 사용 불가
	if (Kart->GetbUsingBooster() == true && Inventory[0].ItemName == EItemName::Booster)
	{
		return;
	}

	// 앞키 안누르고 부스터 누르면 사용 불가
	if (Kart->GetAccelerationComponent()->GetTargetAcceleration() == 0 && Inventory[0].ItemName == EItemName::Booster)
	{
		return;
	}

	// 쉴드 사용 중일때 쉴드 사용 불가
	if (Kart->GetItemInteractionComponent()->bShieldOn == true && Inventory[0].ItemName == EItemName::Shield)
	{
		return;
	}
	
	Server_UseItem();
}

void UItemInventoryComponent::Server_UseItem_Implementation()
{
	SpawnItem(Inventory[0]);
	NetMulticast_UseItem();
}

void UItemInventoryComponent::NetMulticast_UseItem_Implementation()
{
	ARacePlayerController* pc = Cast<ARacePlayerController>(Kart->GetController());
	if (pc)
	{
		UMainUI* mainUI = pc->GetMainHUD();
		if (mainUI)
		{
			UItemInventory* inventoryUI = mainUI->GetWBP_ItemInventory();
			if (inventoryUI)
			{
				inventoryUI->UseItemUI();
			}
		}
	}

	if (Inventory[0].ItemType)
	{
		Kart->GetUsingAimComponent()->SetVisibility(false);
	}
	
	Inventory.RemoveAt(0);
	bInventoryIsFull = false;
}

void UItemInventoryComponent::LockPlayer()
{
	if (Inventory.Num() == 0)
	{
		return;
	}
	
	if (Inventory[0].ItemType == false)
	{
		return;
	}

	FVector start = Kart->GetRootComponent()->GetComponentLocation();
	FVector end = Kart->GetRootComponent()->GetComponentLocation() + Kart->GetRootComponent()->GetForwardVector() * MaxLockOnDist;

	FVector BoxHalfSize(FVector(100.f,100.f,100.f));
	FindTargetAndTakeAim(start, end, BoxHalfSize);
}

void UItemInventoryComponent::SpawnItem(const FItemTable itemData)
{
	// 아이템 스폰
	FTransform itemTransform;
	itemTransform.SetLocation(Kart->GetActorLocation() + Kart->GetActorForwardVector() * 1000.0f);
	itemTransform.SetRotation(Kart->GetActorRotation().Quaternion());
	itemTransform.SetScale3D(FVector(1.0f));
	NetMulticast_PlayItemSound(itemData.ItemName);
	switch (itemData.ItemName)
	{
	case EItemName::Missile:
		{
			if (LockedTarget != nullptr)
			{
				itemTransform.SetLocation(Kart->GetActorLocation() + Kart->GetActorForwardVector() * 100.0f);
				auto* missile = GetWorld()->SpawnActor<AMissile>(itemData.ItemClass, itemTransform);
				missile->SetLockOnPlayer(LockedTarget);
				missile->SetOwningPlayer(Kart);
				LockedTarget = nullptr;
			}
			else if (LockedTarget == nullptr)
			{
				FFastLogger::LogConsole(TEXT("조준 타겟이 nullptr입니다"));
			}
			break;
		}
	case EItemName::WaterBomb:
		{
			itemTransform.SetLocation(Kart->GetActorLocation() + Kart->GetActorUpVector() * 100.f);
			auto* waterBomb = GetWorld()->SpawnActor<AWaterBomb>(itemData.ItemClass, itemTransform);
			if (waterBomb)
			{
				waterBomb->SetOwningPlayer(Kart);
			}
			break;
		}
	case EItemName::Booster:
		{
			auto* booster = GetWorld()->SpawnActor<ABooster>(itemData.ItemClass, itemTransform);
			if (booster)
			{
				booster->SetOwningPlayer(Kart);
			}
			break;
		}
	case EItemName::Shield:
		{
			auto* shield = GetWorld()->SpawnActor<AShield>(itemData.ItemClass, itemTransform);
			if (shield)
			{
				shield->SetOwningPlayer(Kart);
			}
		}
	default:
		break;
	}
}

void UItemInventoryComponent::FindTargetAndTakeAim(FVector start, FVector end, FVector boxHalfSize)
{
	Server_FindTarget(start, end, boxHalfSize);
}

void UItemInventoryComponent::Server_FindTarget_Implementation(FVector start, FVector end, FVector boxHalfSize)
{
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(GetOwner());
	Params.AddIgnoredActors(IgnoredActors);

	FinalTarget = nullptr;
	// 앞에 있는 카트 중에 가장 가까운 카트를 찾는 로직
	float ClosestDistance = TNumericLimits<float>::Max();

	TArray<FHitResult> InitialHitResults;
	FVector InitialBoxSize = boxHalfSize;
	InitialBoxSize.Y *= InitialBoxSizeOffset;

	bool bInitialHit = GetWorld()->SweepMultiByChannel(
		InitialHitResults,
		start,
		end,
		Kart->GetRootComponent()->GetComponentQuat(),
		ECC_Pawn,
		FCollisionShape::MakeBox(InitialBoxSize),
		Params);


	if (bInitialHit)
	{
		for (const FHitResult& Hit : InitialHitResults)
		{
			AKart* PotentialTarget = Cast<AKart>(Hit.GetActor());
			if (PotentialTarget)
			{
				float Distance = FVector::Distance(start, Hit.ImpactPoint);
				if (Distance < ClosestDistance)
				{
					ClosestDistance = Distance;
					FinalTarget = PotentialTarget;
				}
			}
		}
	}
	else
	{
		FinalTarget = nullptr;
	}

	// 박스의 scale y 보간
	float MinYScale = 0.5f;
	float MaxYScale = 4.0f;
	float DistanceScale = MaxYScale;

	if (FinalTarget)
	{
		DistanceScale = FMath::Lerp(MinYScale, MaxYScale, ClosestDistance / MaxLockOnDist);
	}

	FVector AdjustedBoxHalfSize = boxHalfSize;
	AdjustedBoxHalfSize.Y *= DistanceScale;
	
	// 조정된 박스크기로 실제 트레이스 실행
	TArray<FHitResult> HitResults;
	bool bHit = GetWorld()->SweepMultiByChannel(
		HitResults,
		start,
		end,
		Kart->GetRootComponent()->GetComponentQuat(),
		ECC_Pawn,
		FCollisionShape::MakeBox(AdjustedBoxHalfSize),
		Params);

	FColor BoxColor = FColor::Green;
	if (bHit)
	{
		for (const FHitResult hit : HitResults)
		{
			auto* lockedTarget = Cast<AKart>(hit.GetActor());
			if (lockedTarget)
			{
				BoxColor = FColor::Red;
				LockedTarget = lockedTarget;
			}
			else
			{
				BoxColor = FColor::Green;
				LockedTarget = nullptr;
			}
		}
	}
	else
	{
		BoxColor = FColor::Green;
		LockedTarget = nullptr;
	}

	// 조준 사운드
	if (LockedTarget != nullptr)
	{
		NetMulticast_PlayMissileAimSound(true);
	}
	else
	{
		NetMulticast_PlayMissileAimSound(false);
	}
	
	NetMulticast_TakeAim(start, end, AdjustedBoxHalfSize, BoxColor);
}

void UItemInventoryComponent::NetMulticast_TakeAim_Implementation(FVector start, FVector end,
	FVector boxHalfSize, FColor BoxColor)
{
	//DrawAimLineBox(start, end, boxHalfSize, BoxColor);

	if (Kart->IsLocallyControlled())
	{
		Kart->GetUsingAimComponent()->SetVisibility(true);
	}
	SetUsingAimLocation();
}

void UItemInventoryComponent::DrawAimLineBox(FVector start, FVector end, FVector boxHalfSize, FColor BoxColor)
{
	// 서버만 보이도록 설정
	if (Kart->HasAuthority() == false) return;
	
	int NumSteps = 10;
	for (int i = 0; i <= NumSteps; i++)
	{
		float Alpha = (float)i / NumSteps;
		FVector DebugLocation = FMath::Lerp(start, end, Alpha);
	
		DrawDebugBox(GetWorld(), DebugLocation, boxHalfSize, Kart->GetRootComponent()->GetComponentQuat(), BoxColor, false, 0.1f);
	}
}

void UItemInventoryComponent::SetUsingAimLocation()
{
	auto* aim = Kart->GetUsingAimComponent();

	if (aim)
	{
		// 조준 성공했을 때는 에임 고정
		if (LockedTarget != nullptr)
		{
			// 에임 색을 빨강으로
			ChangeAimColor(true);
			
			NetMulticast_SetUsingAimLocation(aim, true, LockedTarget->GetTargetAimSocketComponent()->GetComponentLocation(), FVector(0.4f));
			
		}
		// 조준 실패 중일 때는 에임을 정면 방향으로 설정
		else
		{
			// 에임 색을 초록으로
			ChangeAimColor(false);

			// 주시 중인 상대가 없으면 내 카트 정면에 에임 위치
			if (FinalTarget == nullptr)
			{
				NetMulticast_SetUsingAimLocation(aim, false, InitialAimUIPos, InitialAimUIScale);
			}
			// 에임의 거리는 현재 주시중인 타겟이 내 앞 방향에 가까워짐에 비례하게 에임의 거리가 멀어진다.
			else
			{
				Server_CalcAimLocation(aim);
			}
		}
	}
}

void UItemInventoryComponent::Server_CalcAimLocation_Implementation(class UWidgetComponent* aim)
{
	FVector forward = Kart->GetActorForwardVector();
	FVector rightVector = Kart->GetActorRightVector();
	FVector targetLocation = FinalTarget->GetActorLocation();
	FVector kartLocation = Kart->GetActorLocation();

	FVector direction = (targetLocation - kartLocation).GetSafeNormal();
	float rightDot = FVector::DotProduct(rightVector, direction);
	float distance = FVector::Distance(kartLocation, targetLocation);
				
	// 적절한 위치 보정
	// 좌우 위치 보정
	float lateralFactor = 0.2f;
	FVector lateralOffset = rightVector * rightDot * distance * lateralFactor;
	// 전방 위치 보정
	float forwardFactor = 0.5f;
	FVector forwardOffset = forward * distance * forwardFactor;

	FVector newPos = kartLocation + forwardOffset + lateralOffset;
	// Z 값 유지
	newPos.Z = InitialAimUIPos.Z;

	NetMulticast_SetUsingAimLocation(aim, true, newPos, FVector(0.4f));
}

void UItemInventoryComponent::NetMulticast_SetUsingAimLocation_Implementation(class UWidgetComponent* aim, bool bIsWorldPos, FVector pos, FVector scale)
{
	if (bIsWorldPos)
	{
		aim->SetWorldLocation(pos);
	}
	else
	{
		aim->SetRelativeLocation(pos);
	}
	aim->SetRelativeScale3D(scale);
}

void UItemInventoryComponent::ChangeAimColor(bool bIsLockOn)
{
	auto* aimWidgetComp = Kart->GetUsingAimComponent();
	if (aimWidgetComp)
	{
		UUserWidget* widgetInst = Cast<UUserWidget>(aimWidgetComp->GetWidget());

		if (widgetInst)
		{
			UAim* aimClass = Cast<UAim>(widgetInst);

			if (bIsLockOn)
			{
				aimClass->AimImage->SetBrushFromTexture(aimClass->RedAim);
			}
			else
			{
				aimClass->AimImage->SetBrushFromTexture(aimClass->GreenAim);
			}
		}
	}
}

void UItemInventoryComponent::PlayItemSound(EItemName item)
{
	NetMulticast_PlayItemSound(item);
}

void UItemInventoryComponent::NetMulticast_PlayItemSound_Implementation(EItemName item)
{
	if (Kart->IsLocallyControlled() == false) return;

	auto* itemSoundComponent = Kart->GetItemSoundComponent();
	switch (item)
	{
	case EItemName::Shield:
		{
			itemSoundComponent->PlayShieldSound();
			break;
		}
	case EItemName::WaterBomb:
		{
			itemSoundComponent->PlayWaterBombThrowSound();
			break;
		}
	case EItemName::ItemBox:
		{
			itemSoundComponent->PlayGetItemSound();
			break;
		}
	case EItemName::Missile:
		{
			if (LockedTarget != nullptr)
			{
				itemSoundComponent->PlayMissileMoveSound();
			}
			else
			{
				itemSoundComponent->PlayAimMissSound();
			}
			break;
		}
	default:
		{
			break;
		}
	}
}

void UItemInventoryComponent::NetMulticast_PlayMissileAimSound_Implementation(bool bIsLockOn)
{
	if (Kart->IsLocallyControlled() == false) return;


	auto* ItemSoundComponent = Kart->GetItemSoundComponent();
	if (bIsLockOn)
	{
		if (ItemSoundComponent->GetSound() == ItemSoundComponent->GetMissileLockOnSoundSource())
		{
			return;
		}
		ItemSoundComponent->PlayMissileLockOnSound();
	}
	else
	{
		if (ItemSoundComponent->GetSound() == ItemSoundComponent->GetMissileFindTargetSoundSource())
		{
			return;
		}
		ItemSoundComponent->PlayMissileFindTargetSound();
	}
}

void UItemInventoryComponent::NetMulticast_StopSound_Implementation()
{
	if (Kart->IsLocallyControlled())
	{
		Kart->GetItemSoundComponent()->Stop();
	}
}
