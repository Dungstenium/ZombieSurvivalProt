// Fill out your copyright notice in the Description page of Project Settings.


#include "BarricadeSpot.h"
#include "Components/BillboardComponent.h" 
#include "Components/StaticMeshComponent.h" 
#include "Kismet/GameplayStatics.h"
#include "ZombieSurvivalProtCharacter.h"


ABarricadeSpot::ABarricadeSpot()
{
	PrimaryActorTick.bCanEverTick = true;

	InitialWoodPlank = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("InitPlank"));
	WoodPlank01 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("1stPlank"));
	WoodPlank02 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("2ndPlank"));
	WoodPlank03 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("3rdPlank"));
	WoodPlank04 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("4thPlank"));

	InitialWoodPlank->SetupAttachment(RootComponent);
	WoodPlank01->SetupAttachment(RootComponent);
	WoodPlank02->SetupAttachment(RootComponent);
	WoodPlank03->SetupAttachment(RootComponent);
	WoodPlank04->SetupAttachment(RootComponent);

	InitialWoodPlank->SetVisibility(false);
	WoodPlank01->SetVisibility(false);
	WoodPlank02->SetVisibility(false);
	WoodPlank03->SetVisibility(false);
	WoodPlank04->SetVisibility(false);
}

void ABarricadeSpot::BeginPlay()
{
	Super::BeginPlay();

	SetActorTickEnabled(false);

	ActualBarricadeLife = 0.0;
	PercentBarricadeLife = ActualBarricadeLife / MaxBarricadeLife;

	EndingPosition[0] = InitialWoodPlank->GetComponentLocation();
	EndingPosition[1] = WoodPlank01->GetComponentLocation();
	EndingPosition[2] = WoodPlank02->GetComponentLocation();
	EndingPosition[3] = WoodPlank03->GetComponentLocation();
	EndingPosition[4] = WoodPlank04->GetComponentLocation();

	EndingRotation[0] = InitialWoodPlank->GetComponentRotation();
	EndingRotation[1] = WoodPlank01->GetComponentRotation();
	EndingRotation[2] = WoodPlank02->GetComponentRotation();
	EndingRotation[3] = WoodPlank03->GetComponentRotation();
	EndingRotation[4] = WoodPlank04->GetComponentRotation();

	for (int32 i : bFinishedAnimation)
	{
		bFinishedAnimation[i] = false;
		bFinishedRotationLap[i] = false;
	}
}

void ABarricadeSpot::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (Player)
	{
		if (Player->PressedInteractButton() && Player->PlayerAction == EPlayerAction::Idle && bPlayerInReach && PercentBarricadeLife < 1.0f)
		{
			ActualBarricadeLife += DeltaSeconds * HealingSpeed;

			bIsBuildingBarricade = true;
			Player->PlayerAction = EPlayerAction::Interacting;
		}
		else if (Player->PressedInteractButton() && Player->PlayerAction == EPlayerAction::Interacting && bIsBuildingBarricade && bPlayerInReach && PercentBarricadeLife < 1.0f)
		{
			ActualBarricadeLife += DeltaSeconds * HealingSpeed;
		}
		else if (!Player->PressedInteractButton() && bPlayerInReach && bIsBuildingBarricade)
		{
			bIsBuildingBarricade = false;
			Player->PlayerAction = EPlayerAction::Idle;
		}

		UpdateBarricadeLife();

		UpdatePlanks(DeltaSeconds);

		if (!bPlayerInReach && bFinishedAnimatingActualPlank && bFinishedAnimatingPreviousPlank)
		{
			SetActorTickEnabled(false);
		}
	}
}

void ABarricadeSpot::UpdateBarricadeLife()
{
	if (PercentBarricadeLife < 1)
	{
		PercentBarricadeLife = ActualBarricadeLife / MaxBarricadeLife;

		if (!NearbyUIElement->IsVisible())
		{
			NearbyUIElement->SetVisibility(true);
		}
	}
	else if (PercentBarricadeLife >= 1.0f && NearbyUIElement->IsVisible())
	{
		NearbyUIElement->SetVisibility(false);
	}
}

void ABarricadeSpot::UpdatePlanks(float DeltaSeconds)
{
	if (PercentBarricadeLife >= 0.2f && !InitialWoodPlank->IsVisible() && InitialWoodPlank)
	{
		ActivateBarricade(0, InitialWoodPlank);
	}
	else if (PercentBarricadeLife < 0.4f && InitialWoodPlank->IsVisible())
	{
		AnimateBarricade(DeltaSeconds, 0, InitialWoodPlank);
	}
	else if (PercentBarricadeLife >= 0.4f && !WoodPlank01->IsVisible() && WoodPlank01)
	{
		ActivateBarricade(1, WoodPlank01);
	}
	else if (PercentBarricadeLife < 0.6f && WoodPlank01->IsVisible())
	{
		AnimateBarricade(DeltaSeconds, 1, WoodPlank01);
	}
	else if (PercentBarricadeLife >= 0.6f && !WoodPlank02->IsVisible() && WoodPlank02)
	{
		ActivateBarricade(2, WoodPlank02);
	}
	else if (PercentBarricadeLife < 0.8f && WoodPlank02->IsVisible())
	{
		AnimateBarricade(DeltaSeconds, 2, WoodPlank02);
	}
	else if (PercentBarricadeLife >= 0.8f && !WoodPlank03->IsVisible() && WoodPlank03)
	{
		ActivateBarricade(3, WoodPlank03);
	}
	else if (PercentBarricadeLife < 1.0f && WoodPlank03->IsVisible())
	{
		AnimateBarricade(DeltaSeconds, 3, WoodPlank03);
	}
	else if (PercentBarricadeLife >= 1.0f && !WoodPlank04->IsVisible() && WoodPlank04)
	{
		ActivateBarricade(4, WoodPlank04);

		Player->PlayerAction = EPlayerAction::Idle;
	}
	else if (PercentBarricadeLife < 1.2f && WoodPlank04->IsVisible())
	{
		AnimateBarricade(DeltaSeconds, 4, WoodPlank04);
	}
}

void ABarricadeSpot::ActivateBarricade(int32 i, UStaticMeshComponent* ActualBarricade)
{
	ActualBarricade->SetVisibility(true);
	ActualBarricade->SetCollisionProfileName(FName("BlockAllDynamic"));

	ActualBarricade->SetWorldLocation(EndingPosition[i] - FVector(0.0f, -75.0f, 0.0f));
	ActualBarricade->AddWorldRotation(FRotator(RotationSpeed, 0.0f, 0.0f));

	UGameplayStatics::PlaySoundAtLocation(this, PlaceBarricadeSound, GetActorLocation());

	//If is not the first plank, consider the previous plank to have his animation finished
	if (i != 0)
	{
		bFinishedAnimatingPreviousPlank = true;
	}

	bFinishedAnimatingActualPlank = false;
}

void ABarricadeSpot::AnimateBarricade(float DeltaSeconds, int32 i, UStaticMeshComponent* ActualBarricade)
{
	if (ActualBarricade->GetComponentRotation().Pitch <= EndingRotation[i].Pitch + 4.0f && ActualBarricade->GetComponentRotation().Pitch >= EndingRotation[i].Pitch - 4.0f && bFinishedRotationLap[i])
	{
		if (ActualBarricade->GetComponentLocation().Y < EndingPosition[i].Y + 5.0f && !bFinishedAnimation[i] && ShakeCamera)
		{
			bFinishedAnimation[i] = true;
			GetWorld()->GetFirstPlayerController()->PlayerCameraManager->PlayCameraShake(ShakeCamera, 1.0f);
			bFinishedAnimatingActualPlank = true;
			bFinishedAnimatingPreviousPlank = true;
		}
		else if (!bFinishedAnimation[i])
		{
			FVector ActualPosition = ActualBarricade->GetComponentLocation();
			ActualPosition.Y -= DeltaSeconds * MoveSpeed;
			ActualBarricade->SetWorldLocation(ActualPosition);
		}
	}
	else
	{
		ActualBarricade->AddWorldRotation(FRotator(RotationSpeed, 0.0f, 0.0f));

		if (ActualBarricade->GetComponentRotation().Pitch <= -70.0f)
		{
			bFinishedRotationLap[i] = true;
		}
	}
}

void ABarricadeSpot::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OnOverlapBegin(OverlappedComp, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);

	if (OtherActor->IsA<AZombieSurvivalProtCharacter>())
	{
		Player = Cast<AZombieSurvivalProtCharacter>(OtherActor);
		SetActorTickEnabled(true);
		bPlayerInReach = true;
	}
}

void ABarricadeSpot::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	Super::OnOverlapEnd(OverlappedComp, OtherActor, OtherComp, OtherBodyIndex);

	if (OtherActor->IsA<AZombieSurvivalProtCharacter>())
	{
		bPlayerInReach = false;

		if (bFinishedAnimatingActualPlank && bFinishedAnimatingPreviousPlank)
		{
			SetActorTickEnabled(false);
		}
	}
}
