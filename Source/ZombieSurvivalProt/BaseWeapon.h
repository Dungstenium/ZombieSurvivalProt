// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BaseWeapon.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ZOMBIESURVIVALPROT_API UBaseWeapon : public UActorComponent
{
	GENERATED_BODY()

private:

	bool bHasAmmo = true;

	UPROPERTY(EditAnywhere)
	class UArrowComponent* GuideArrow;

	UPROPERTY(EditAnywhere)
	float BulletRange{ 3000.0f };

	UPROPERTY()
	class AZombieSurvivalProtCharacter* Player;

	void ReduceAmmoPerShot();

protected:
	virtual void BeginPlay() override;


public:
	UBaseWeapon();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		class USoundBase* FireSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		class UAnimMontage* FireAnimation;

	UPROPERTY(EditAnywhere, Category = Mesh)
		class USkeletalMeshComponent* FirearmMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 ReserveAmmo;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 MaxAmmo { 30 };

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 AmmoCounter;

	void Shoot();
	void Reload();
};
