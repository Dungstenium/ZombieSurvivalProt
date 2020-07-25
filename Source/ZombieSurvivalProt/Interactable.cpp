// Fill out your copyright notice in the Description page of Project Settings.


#include "Interactable.h"
#include "Components/BillboardComponent.h" 
#include "Components/BoxComponent.h" 
#include "Components/SlateWrapperTypes.h" 
#include "Components/StaticMeshComponent.h"
#include "ZombieSurvivalProtCharacter.h"


AInteractable::AInteractable()
{
	PrimaryActorTick.bCanEverTick = false;

	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	RootComponent = StaticMesh;

	Trigger = CreateDefaultSubobject<UBoxComponent>(TEXT("Trigger"));
	Trigger->SetupAttachment(StaticMesh);

	PressUIElement = CreateDefaultSubobject<UBillboardComponent>(TEXT("PopUp"));
	PressUIElement->SetupAttachment(RootComponent);

	NearbyUIElement = CreateDefaultSubobject<UBillboardComponent>(TEXT("Visualizer"));
	NearbyUIElement->SetupAttachment(RootComponent);
}

void AInteractable::BeginPlay()
{
	Super::BeginPlay();
	
	Trigger->OnComponentBeginOverlap.AddDynamic(this, &AInteractable::OnOverlapBegin);
	Trigger->OnComponentEndOverlap.AddDynamic(this, &AInteractable::OnOverlapEnd);
	
	PressUIElement->SetVisibility(false);
	NearbyUIElement->SetVisibility(false);
}

void AInteractable::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor->IsA<AZombieSurvivalProtCharacter>())
	{
		NearbyUIElement->SetVisibility(true);
	}
}

void AInteractable::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor->IsA<AZombieSurvivalProtCharacter>())
	{
		NearbyUIElement->SetVisibility(false);
	}
}

void AInteractable::SetPressUIVisible(bool State)
{
	PressUIElement->SetVisibility(State);

}
