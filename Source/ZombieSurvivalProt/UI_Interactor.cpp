// Fill out your copyright notice in the Description page of Project Settings.


#include "UI_Interactor.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"

#define OUT

// Sets default values for this component's properties
UUI_Interactor::UUI_Interactor()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UUI_Interactor::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UUI_Interactor::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(OUT PlayerViewPointLocation, OUT PlayerViewPointRotation);
	
	UE_LOG(LogTemp, Warning, TEXT("%s & %s"), *PlayerViewPointLocation.ToString(), *PlayerViewPointRotation.ToString())
	// ...
}

