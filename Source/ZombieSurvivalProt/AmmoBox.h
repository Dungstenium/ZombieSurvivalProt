// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interactable.h"
#include "AmmoBox.generated.h"

/**
 * 
 */


UCLASS()
class ZOMBIESURVIVALPROT_API AAmmoBox : public AInteractable
{
	GENERATED_BODY()


	UFUNCTION()
	virtual void OnOverlapEnd(
		class UPrimitiveComponent* OverlappedComp,
		class AActor* OtherActor,
		class UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex) override;

	UFUNCTION()
		virtual void OnOverlapBegin(
			class UPrimitiveComponent* OverlappedComp,
			class AActor* OtherActor,
			class UPrimitiveComponent* OtherComp,
			int32 OtherBodyIndex, bool bFromSweep,
			const FHitResult& SweepResult) override;

	AAmmoBox();


};
