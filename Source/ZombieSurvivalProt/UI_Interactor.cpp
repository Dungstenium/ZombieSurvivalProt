 // Fill out your copyright notice in the Description page of Project Settings.


#include "UI_Interactor.h"
#include "DrawDebugHelpers.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "Interactable.h"

#define OUT

UUI_Interactor::UUI_Interactor()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UUI_Interactor::BeginPlay()
{
	Super::BeginPlay();

}

void UUI_Interactor::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(OUT PlayerViewPointLocation, OUT PlayerViewPointRotation);

	FVector LineTraceEnd = PlayerViewPointLocation + PlayerViewPointRotation.Vector() * LineRange;

	DrawDebugLine(
		GetWorld(),
		PlayerViewPointLocation, 
		LineTraceEnd,
		FColor::Blue,
		false,
		0.0f, 
		0, 
		5.0f);

	FHitResult Hit;
	FCollisionQueryParams TraceParams(FName(""), false, GetOwner());
	GetWorld()->LineTraceSingleByObjectType(
		OUT Hit,
		PlayerViewPointLocation,
		LineTraceEnd,
		FCollisionObjectQueryParams(ECollisionChannel::ECC_PhysicsBody),
		TraceParams);

	AActor* ActorHit = Hit.GetActor();

	if (ActorHit)
	{
		if (ActorHit->IsA<AInteractable>())
		{
			UE_LOG(LogTemp, Warning, TEXT("nigg %s"), *ActorHit->GetName())
		}
	}
}
