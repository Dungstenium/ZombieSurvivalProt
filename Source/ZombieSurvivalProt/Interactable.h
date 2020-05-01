// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interactable.generated.h"

UCLASS()
class ZOMBIESURVIVALPROT_API AInteractable : public AActor
{
	GENERATED_BODY()
	

	UFUNCTION()
		void OnOverlapBegin(
			class UPrimitiveComponent* OverlappedComp,
			class AActor* OtherActor,
			class UPrimitiveComponent* OtherComp, 
			int32 OtherBodyIndex, bool bFromSweep, 
			const FHitResult& SweepResult);

	UFUNCTION()
		void OnOverlapEnd(
			class UPrimitiveComponent* OverlappedComp,
			class AActor* OtherActor, 
			class UPrimitiveComponent* OtherComp,
			int32 OtherBodyIndex);

	//UPROPERTY(EditAnywhere, Category = Properties)
	//	class UPaperSpriteComponent* PresPopUp;

	//UPROPERTY(EditAnywhere, Category = Properties)
	//	class UPaperSpriteComponent* InteractorVisualizer;

public:	
	// Sets default values for this actor's properties
	AInteractable();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, Category = Properties)
		class UStaticMeshComponent* StaticMesh;

	UPROPERTY(EditAnywhere, Category = Properties)
		class UBoxComponent* Trigger;
};
