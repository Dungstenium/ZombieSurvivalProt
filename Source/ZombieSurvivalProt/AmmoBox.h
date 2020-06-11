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

	virtual void Tick(float DeltaSeconds) override;
	virtual void BeginPlay() override;

	bool PlayerInReach = false;

	float Timer = 0.0f;

	class AZombieSurvivalProtCharacter* Player = NULL;

protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	class USoundBase* FireSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	class UAnimMontage* RearmAnimation;
};
