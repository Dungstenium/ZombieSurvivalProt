// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AmmoCrate.generated.h"

UCLASS()
class ZOMBIESURVIVALPROT_API AAmmoCrate : public AActor
{
	GENERATED_BODY()


	UPROPERTY(EditAnywhere)
	class UStaticMeshComponent* StaticMesh;

	UPROPERTY(EditAnywhere)
	class UBoxComponent* CrateTrigger;

protected:
	virtual void BeginPlay() override;

public:	
	AAmmoCrate();
	

};
