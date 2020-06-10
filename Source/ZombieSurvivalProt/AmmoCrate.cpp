// Fill out your copyright notice in the Description page of Project Settings.


#include "AmmoCrate.h"
#include "Components/BoxComponent.h" 
#include "Components/StaticMeshComponent.h" 


AAmmoCrate::AAmmoCrate()
{
	PrimaryActorTick.bCanEverTick = false;

	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("AmmoCrateMesh"));
	RootComponent = StaticMesh;

}

void AAmmoCrate::BeginPlay()
{
	Super::BeginPlay();
	
}

