// Fill out your copyright notice in the Description page of Project Settings.


#include "AmmoBox.h"
#include "BaseWeapon2.h"
#include "Components/BoxComponent.h" 
#include "Components/InputComponent.h"
#include "ZombieSurvivalProtCharacter.h"


AAmmoBox::AAmmoBox()
{
	PrimaryActorTick.bCanEverTick = true;

}

void AAmmoBox::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (Player && Player->GetPlayerInteraction() && PlayerInReach)
	{
		Player->EquipedRifle->ReplenishAmmo();	
		UE_LOG(LogTemp, Warning, TEXT("sup2"))

		Player->DeactivateInteractionWithObject();
	}
}

void AAmmoBox::BeginPlay()
{
	Super::BeginPlay();

	Trigger->OnComponentBeginOverlap.AddDynamic(this, &AAmmoBox::OnOverlapBegin);
	Trigger->OnComponentEndOverlap.AddDynamic(this, &AAmmoBox::OnOverlapEnd);
}

void AAmmoBox::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OnOverlapBegin(OverlappedComp, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);

	if (OtherActor->IsA<AZombieSurvivalProtCharacter>())
	{
		Player = Cast<AZombieSurvivalProtCharacter>(OtherActor);
		PlayerInReach = true;
		UE_LOG(LogTemp,Warning, TEXT("entered"))
	}
}

void AAmmoBox::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	Super::OnOverlapEnd(OverlappedComp, OtherActor, OtherComp, OtherBodyIndex);

	if (OtherActor->IsA<AZombieSurvivalProtCharacter>())
	{
		PlayerInReach = false;
		UE_LOG(LogTemp, Warning, TEXT("left"))
	}
}
