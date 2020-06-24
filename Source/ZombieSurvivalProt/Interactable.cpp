// Fill out your copyright notice in the Description page of Project Settings.


#include "Interactable.h"
#include "Components/BillboardComponent.h" 
#include "Components/BoxComponent.h" 
#include "Components/StaticMeshComponent.h"
#include "ZombieSurvivalProtCharacter.h"


AInteractable::AInteractable()
{
	PrimaryActorTick.bCanEverTick = false;

	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	RootComponent = StaticMesh;

	Trigger = CreateDefaultSubobject<UBoxComponent>(TEXT("Trigger"));
	Trigger->SetupAttachment(StaticMesh);

	PressPopUp = CreateDefaultSubobject<UBillboardComponent>(TEXT("PopUp"));
	PressPopUp->SetupAttachment(RootComponent);

	InteractableVisualizer = CreateDefaultSubobject<UBillboardComponent>(TEXT("Visualizer"));
	InteractableVisualizer->SetupAttachment(RootComponent);
}

void AInteractable::BeginPlay()
{
	Super::BeginPlay();
	
	Trigger->OnComponentBeginOverlap.AddDynamic(this, &AInteractable::OnOverlapBegin);
	Trigger->OnComponentEndOverlap.AddDynamic(this, &AInteractable::OnOverlapEnd);
	
	PressPopUp->SetVisibility(false);
	InteractableVisualizer->SetVisibility(false);
}

void AInteractable::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor->IsA<AZombieSurvivalProtCharacter>())
	{
		InteractableVisualizer->SetVisibility(true);
	}
}

void AInteractable::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor->IsA<AZombieSurvivalProtCharacter>())
	{
		InteractableVisualizer->SetVisibility(false);
	}
}

void AInteractable::SetPressPopUpVisible(bool State)
{
	PressPopUp->SetVisibility(State);

}
