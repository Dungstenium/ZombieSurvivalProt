// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BaseWeapon2.generated.h"

UCLASS()
class ZOMBIESURVIVALPROT_API ABaseWeapon2 : public AActor
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

	UPROPERTY(EditAnywhere, Category = Gameplay)
	class UParticleSystem* MuzzleFlash;

protected:
	virtual void BeginPlay() override;


public:
	ABaseWeapon2();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	class USoundBase* FireSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	class UAnimMontage* FireAnimation;

	UPROPERTY(EditAnywhere, Category = Mesh)
	class USkeletalMeshComponent* FirearmMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 ReserveAmmo;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 WeaponMagazinSize{30};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 AmmoCounter;

	UPROPERTY(EditAnywhere)
	float DamagePerShot{ 10.0f };

	void Shoot();
	void Reload();
};
