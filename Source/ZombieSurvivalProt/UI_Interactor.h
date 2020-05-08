// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "UI_Interactor.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ZOMBIESURVIVALPROT_API UUI_Interactor : public UActorComponent
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
		float LineRange = 200.0f;

	class AInteractable* InteractingActor = nullptr;

	UPROPERTY(EditAnywhere)
		bool bActivateDebugRay = false;

public:	
	UUI_Interactor();

protected:
	virtual void BeginPlay() override;

	FVector PlayerViewPointLocation;
	FRotator PlayerViewPointRotation;

	bool bIsPopUpVisible{ false };

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
		
};
