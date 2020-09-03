// Fill out your copyright notice in the Description page of Project Settings.


#include "LifeManager.h"


ULifeManager::ULifeManager()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void ULifeManager::BeginPlay()
{
	Super::BeginPlay();

	ActualLife = MaxLife;
}

void ULifeManager::DealDamage(const float Damage)
{
	ActualLife -= Damage;
	
	if (ActualLife <= 0.0f)
	{
		Die();
	}
}

void ULifeManager::Heal(const float Amount)
{
	ActualLife += Amount;
}

float ULifeManager::GetLife() const
{
	return ActualLife;
}

void ULifeManager::Die()
{
	UE_LOG(LogTemp, Warning, TEXT("Player died"));
}
