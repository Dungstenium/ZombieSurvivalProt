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

	bool bFinishedAnimation[5];
	bool bFinishedRotationLap[5];
	bool bFinishedAnimatingActualPlank = true;
	bool bFinishedAnimatingPreviousPlank = true;

	float MaxBarricadeLife = 100.0f;
	float ActualBarricadeLife;
	float PercentBarricadeLife;

	UPROPERTY(EditAnywhere, Category = Properties)
	float HealingSpeed = 10.0f;

	UPROPERTY(EditAnywhere, Category = Properties)
	float RotationSpeed = 5.0f;

	UPROPERTY(EditAnywhere, Category = Properties)
	float MoveSpeed = 370.0f;

	UPROPERTY(EditAnywhere)
	FVector BarricadeOffset = FVector(0.0f, -75.0f, 0.0f);

	FVector EndingPosition[5];
	FRotator EndingRotation[5];

	UPROPERTY(VisibleAnywhere)
	class UStaticMeshComponent* InitialWoodPlank;

	UPROPERTY(VisibleAnywhere)
	class UStaticMeshComponent* WoodPlank01;

	UPROPERTY(VisibleAnywhere)
	class UStaticMeshComponent* WoodPlank02;

	UPROPERTY(VisibleAnywhere)
	class UStaticMeshComponent* WoodPlank03;

	UPROPERTY(VisibleAnywhere)
	class UStaticMeshComponent* WoodPlank04;

	class AZombieSurvivalProtCharacter* Player = NULL;

	bool bPlayerInReach = false;
	bool bIsBuildingBarricade = false;

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaSeconds) override;

	void UpdatePlanks(float DeltaSeconds);

	void UpdateBarricadeLife();

	void ActivateBarricade(int32 i, UStaticMeshComponent* ActualBarricade);

	void AnimateBarricade(float DeltaSeconds, int32 i, UStaticMeshComponent* ActualBarricade);

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Reloading)
	class USoundBase* PlaceBarricadeSound;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UCameraShake> ShakeCamera;
};
