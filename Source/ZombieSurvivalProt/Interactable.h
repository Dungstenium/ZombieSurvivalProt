// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interactable.generated.h"

UCLASS()
class ZOMBIESURVIVALPROT_API AInteractable : public AActor
{
	GENERATED_BODY()
	

protected:

		virtual void OnOverlapBegin(
			class UPrimitiveComponent* OverlappedComp,
			class AActor* OtherActor,
			class UPrimitiveComponent* OtherComp, 
			int32 OtherBodyIndex, bool bFromSweep, 
			const FHitResult& SweepResult);

		virtual void OnOverlapEnd(
			class UPrimitiveComponent* OverlappedComp,
			class AActor* OtherActor, 
			class UPrimitiveComponent* OtherComp,
			int32 OtherBodyIndex);

	UPROPERTY(EditAnywhere, Category = Properties)
	class UBillboardComponent* PressPopUp;

	UPROPERTY(EditAnywhere, Category = Properties)
	class UBillboardComponent* InteractableVisualizer;

	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, Category = Properties)
	class UStaticMeshComponent* StaticMesh;

	UPROPERTY(EditAnywhere, Category = Properties)
	class UBoxComponent* Trigger;

public:	

	AInteractable();

	void SetPressPopUpVisible(bool State);
};
