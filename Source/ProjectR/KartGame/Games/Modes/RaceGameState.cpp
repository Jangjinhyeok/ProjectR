#include "RaceGameState.h"

#include "Kismet/GameplayStatics.h"
#include "ProjectR/KartGame/Games/Objects/CheckPoint.h"
#include "ProjectR/KartGame/Games/Modes/RiderPlayerState.h"

ARaceGameState::ARaceGameState()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ARaceGameState::BeginPlay()
{
	Super::BeginPlay();
	
	TArray<AActor*> CheckPointList;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACheckPoint::StaticClass(), CheckPointList);

	if (CheckPointList.IsEmpty())
	{
		UE_LOG(LogTemp, Error, TEXT("현재 잘못된 맵 세팅입니다. 재확인이 필요합니다: 사유 체크포인트 존재하지 않음"))
		return;
	}
	
	Algo::HeapSort(CheckPointList, [](const AActor* A, const AActor* B)
	{
		const ACheckPoint* CheckPointA = static_cast<const ACheckPoint*>(A);
		const ACheckPoint* CheckPointB = static_cast<const ACheckPoint*>(B);
		
		return CheckPointA->GetPinMainNumber() < CheckPointB->GetPinMainNumber();
	});

	// ReadOnly용 체크 포인트 데이터 가져오기
	for (AActor* CheckPoint : CheckPointList)
	{
		ACheckPoint* NewCheckPoint = static_cast<ACheckPoint*>(CheckPoint);
		CheckPointData.Add(NewCheckPoint->GetCurrentCheckPoint(), NewCheckPoint);
	}

	const ACheckPoint* LastPoint = static_cast<ACheckPoint*>(CheckPointList[CheckPointList.Num() - 1]);
	// 체크포인트 최대 숫자는 보통 0 ~ 최대 숫자 까지기 때문에 0 ~ 최대 숫자 만큼 보다
	// 적은 갯수를 보유하고 있으면 맵에 있는 체크포인트 자체에 문제가 있음을 의미한다.
	if (CheckPointList.Num() < LastPoint->GetPinMainNumber() + 1)
	{
		UE_LOG(LogTemp, Error, TEXT("현재 잘못된 맵 세팅입니다. 재확인이 필요합니다: 사유 체크포인트 갯수 부족"))
	}

	MaxCheckPoint = LastPoint->GetPinMainNumber();
}

void ARaceGameState::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	Algo::HeapSort(PlayerArray, [](const APlayerState* A, const APlayerState* B)
	{
		const ARiderPlayerState* PSA = Cast<ARiderPlayerState>(A);
		const ARiderPlayerState* PSB = Cast<ARiderPlayerState>(B);

		if (PSA->GetCurrentLap() != PSB->GetCurrentLap())
		{
			return PSA->GetCurrentLap() > PSB->GetCurrentLap();
		}

		// 세부 체크포인트 끼리에 대한 것은 아래에서 처리할 수 있다.
		// 우선은 세부의 세부로 들어갈 일은 없어 당장은 간단하게 처리하고 생각한다.
		if (PSA->GetCurrentKartCheckPoint() != PSB->GetCurrentKartCheckPoint())
		{
			return PSA->GetCurrentMainCheckPoint() > PSB->GetCurrentMainCheckPoint();
		}

		// 여기서부터는 수식이 들어간다. 가장 가까운 다음 체크포인트 정보를 비교하는 방식으로 처리한다.
		return true;
	});
}