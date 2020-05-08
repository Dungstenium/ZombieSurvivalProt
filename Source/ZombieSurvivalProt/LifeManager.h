// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "LifeManager.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ZOMBIESURVIVALPROT_API ULifeManager : public UActorComponent
{
	GENERATED_BODY()


	UPROPERTY(EditAnywhere)
		float MaxLife{ 100.0f };

	UPROPERTY()
		float ActualLife{ 100.0f };

public:	
	ULifeManager();

protected:
	virtual void BeginPlay() override;

public:	
	void DealDamage(float Damage);
	void Heal(float Amount);
	float GetLife() const;
	void Die();
};
