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
	}
}

void ABarricadeSpot::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (Player)
	{
		if (Player->PlayerAction == EPlayerAction::Idle && Player->PlayerIsInteracting() && bPlayerInReach && PercentBarricadeLife < 1.0f)
		{
			ActualBarricadeLife += DeltaSeconds * 10.0f;

			bIsBuildingBarricade = true;
			Player->PlayerAction = EPlayerAction::Interacting;
		}
		else if (Player->PlayerAction == EPlayerAction::Interacting && bIsBuildingBarricade && Player->PlayerIsInteracting() && bPlayerInReach && PercentBarricadeLife < 1.0f)
		{
			ActualBarricadeLife += DeltaSeconds * 10.0f;
		}
		else if (!Player->PlayerIsInteracting() && bPlayerInReach && bIsBuildingBarricade)
		{
			bIsBuildingBarricade = false;
			Player->PlayerAction = EPlayerAction::Idle;
		}

		if (PercentBarricadeLife < 1)
		{
			PercentBarricadeLife = ActualBarricadeLife / MaxBarricadeLife;
			
			if (!InteractableVisualizer->IsVisible())
			{
				InteractableVisualizer->SetVisibility(true);
			}
		}

		if(PercentBarricadeLife >= 0.2f && InitialWoodPlank && !InitialWoodPlank->IsVisible())
		{
			InitialWoodPlank->SetVisibility(true);
			InitialWoodPlank->SetCollisionProfileName(FName("BlockAllDynamic"));

			InitialWoodPlank->SetWorldLocation(EndingPosition[0] - BarricadeOffset);
			InitialWoodPlank->AddWorldRotation(FRotator(5.0f, 0.0f, 0.0f));

			UE_LOG(LogTemp, Warning, TEXT("%s"), *InitialWoodPlank->GetComponentRotation().ToString())

			UGameplayStatics::PlaySoundAtLocation(this, PlaceBarricadeSound, GetActorLocation());
		}
		else if (PercentBarricadeLife < 0.4f && InitialWoodPlank->IsVisible())
		{
			if (InitialWoodPlank->GetComponentRotation().Pitch <= EndingRotation[0].Pitch + 4.0f && InitialWoodPlank->GetComponentRotation().Pitch >= EndingRotation[0].Pitch - 4.0f)
			{

				if (InitialWoodPlank->GetComponentLocation().Y < EndingPosition[0].Y + 5.0f && !bFinishedAnimation[0] && ShakeCamera)
				{
					UE_LOG(LogTemp, Warning, TEXT("aaa"));
					bFinishedAnimation[0] = true;
					GetWorld()->GetFirstPlayerController()->PlayerCameraManager->PlayCameraShake(ShakeCamera, 1.0f);
				}
				else if (!bFinishedAnimation[0])
				{
					FVector ActualPosition = InitialWoodPlank->GetComponentLocation();
					ActualPosition.Y = FMath::FInterpConstantTo(ActualPosition.Y, EndingPosition[0].Y, DeltaSeconds, 400.0f);
					InitialWoodPlank->SetWorldLocation(ActualPosition);
					UE_LOG(LogTemp, Warning, TEXT("Acutal:%f, Ending: %f"), ActualPosition.Y, EndingPosition[0].Y);

				}
			}
			else
			{
				InitialWoodPlank->AddWorldRotation(FRotator(5.0f, 0.0f, 0.0f));
			}
		}
		else if (PercentBarricadeLife >= 0.4f && WoodPlank01 && !WoodPlank01->IsVisible())
		{
			WoodPlank01->SetVisibility(true);
			WoodPlank01->SetCollisionProfileName(FName("BlockAllDynamic"));

			UGameplayStatics::PlaySoundAtLocation(this, PlaceBarricadeSound, GetActorLocation());
		}
		else if (PercentBarricadeLife >= 0.6f && WoodPlank02 && !WoodPlank02->IsVisible())
		{
			WoodPlank02->SetVisibility(true);
			WoodPlank02->SetCollisionProfileName(FName("BlockAllDynamic"));

			UGameplayStatics::PlaySoundAtLocation(this, PlaceBarricadeSound, GetActorLocation());
		}
		else if (PercentBarricadeLife >= 0.8f && WoodPlank03 && !WoodPlank03->IsVisible())
		{
			WoodPlank03->SetVisibility(true);
			WoodPlank03->SetCollisionProfileName(FName("BlockAllDynamic"));

			UGameplayStatics::PlaySoundAtLocation(this, PlaceBarricadeSound, GetActorLocation());
		}
		else if (PercentBarricadeLife >= 1.0f && WoodPlank04 && !WoodPlank04->IsVisible())
		{
			WoodPlank04->SetVisibility(true);
			WoodPlank04->SetCollisionProfileName(FName("BlockAllDynamic"));

			UGameplayStatics::PlaySoundAtLocation(this, PlaceBarricadeSound, GetActorLocation());

			InteractableVisualizer->SetVisibility(false);

			Player->PlayerAction = EPlayerAction::Idle;
		}
		else if (PercentBarricadeLife >= 1.0f && InteractableVisualizer->IsVisible())
		{
			InteractableVisualizer->SetVisibility(false);
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
		SetActorTickEnabled(false);
		bPlayerInReach = false;
	}
}
