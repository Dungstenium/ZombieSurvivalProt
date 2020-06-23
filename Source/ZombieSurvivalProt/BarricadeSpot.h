// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interactable.h"
#include "BarricadeSpot.generated.h"

/**
 * 
 */
UCLASS()
class ZOMBIESURVIVALPROT_API ABarricadeSpot : public AInteractable
{
	GENERATED_BODY()


	ABarricadeSpot();

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

	float MaxBarricadeLife = 100.0f;
	float ActualBarricadeLife;
	float PercentBarricadeLife;

	UPROPERTY(VisibleAnywhere)
	class UStaticMeshComponent* WoodPlank01;

	UPROPERTY(VisibleAnywhere)
	class UStaticMeshComponent* WoodPlank02;

	UPROPERTY(VisibleAnywhere)
	class UStaticMeshComponent* WoodPlank03;

	UPROPERTY(VisibleAnywhere)
	class UStaticMeshComponent* WoodPlank04;

	UPROPERTY(VisibleAnywhere)
	class UStaticMeshComponent* WoodPlank05;

	class AZombieSurvivalProtCharacter* Player = NULL;

	bool bPlayerInReach = false;

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaSeconds) override;

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Reloading)
	class USoundBase* PlaceBarricadeSound;
};
