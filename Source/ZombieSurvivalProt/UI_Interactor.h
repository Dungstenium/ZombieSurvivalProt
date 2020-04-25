// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "UI_Interactor.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ZOMBIESURVIVALPROT_API UUI_Interactor : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UUI_Interactor();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	FVector PlayerViewPointLocation;
	FRotator PlayerViewPointRotation;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
