// Fill out your copyright notice in the Description page of Project Settings.


#include "BarricadeSpot.h"
#include "Components/StaticMeshComponent.h" 
#include "ZombieSurvivalProtCharacter.h"


ABarricadeSpot::ABarricadeSpot()
{
	PrimaryActorTick.bCanEverTick = true;

	WoodPlank01 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("1stPlank"));
	WoodPlank02 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("2ndPlank"));
	WoodPlank03 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("3rdPlank"));
	WoodPlank04 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("4thPlank"));
	WoodPlank05 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("5thPlank"));

	WoodPlank01->SetupAttachment(RootComponent);
	WoodPlank02->SetupAttachment(RootComponent);
	WoodPlank03->SetupAttachment(RootComponent);
	WoodPlank04->SetupAttachment(RootComponent);
	WoodPlank05->SetupAttachment(RootComponent);

	WoodPlank01->SetVisibility(false);
	WoodPlank02->SetVisibility(false);
	WoodPlank03->SetVisibility(false);
	WoodPlank04->SetVisibility(false);
	WoodPlank05->SetVisibility(false);
}

void ABarricadeSpot::BeginPlay()
{
	Super::BeginPlay();

	SetActorTickEnabled(false);

	ActualBarricadeLife = 0.0;
	PercentBarricadeLife = ActualBarricadeLife / MaxBarricadeLife;
}

void ABarricadeSpot::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (Player)
	{
		if (Player->PlayerAction == EPlayerAction::Idle && Player->GetPlayerInteraction() && bPlayerInReach)
		{
			ActualBarricadeLife += DeltaSeconds * 10.0f;
		}

		if (PercentBarricadeLife < 1)
		{
			PercentBarricadeLife = ActualBarricadeLife / MaxBarricadeLife;
		}

		if(PercentBarricadeLife >= 0.2f && WoodPlank01 && !WoodPlank01->IsVisible())
		{
			WoodPlank01->SetVisibility(true);
			WoodPlank01->SetCollisionProfileName(FName("BlockAllDynamic"));
		}
		else if (PercentBarricadeLife >= 0.4f && WoodPlank02 && !WoodPlank02->IsVisible())
		{
			WoodPlank02->SetVisibility(true);
			WoodPlank02->SetCollisionProfileName(FName("BlockAllDynamic"));
		}
		else if (PercentBarricadeLife >= 0.6f && WoodPlank03 && !WoodPlank03->IsVisible())
		{
			WoodPlank03->SetVisibility(true);
			WoodPlank03->SetCollisionProfileName(FName("BlockAllDynamic"));
		}
		else if (PercentBarricadeLife >= 0.8f && WoodPlank04 && !WoodPlank04->IsVisible())
		{
			WoodPlank04->SetVisibility(true);
			WoodPlank04->SetCollisionProfileName(FName("BlockAllDynamic"));
		}
		else if (PercentBarricadeLife >= 1.0f && WoodPlank05 && !WoodPlank05->IsVisible())
		{
			WoodPlank05->SetVisibility(true);
			WoodPlank05->SetCollisionProfileName(FName("BlockAllDynamic"));
		}

		UE_LOG(LogTemp, Warning, TEXT("%f"), PercentBarricadeLife);
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
		SetActorTickEnabled(false);
		bPlayerInReach = false;
	}
}
