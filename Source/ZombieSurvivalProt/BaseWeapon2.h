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

	float Timer = 0.0f;

	UPROPERTY(EditAnywhere)
	float BulletRange{ 3000.0f };

	UPROPERTY(EditAnywhere)
	class UArrowComponent* GuideArrow;

	UPROPERTY()
	class AZombieSurvivalProtCharacter* Player;

	void ReduceAmmoPerShot();

	UPROPERTY(EditAnywhere, Category = Gameplay)
	class UParticleSystem* MuzzleFlash;

	UPROPERTY(EditAnywhere, Category = Gameplay)
	class UParticleSystem* BulletImpactOnWalls;

	UPROPERTY(EditAnywhere, Category = Gameplay)
	class UParticleSystem* BulletImpactOnZombies;

	UPROPERTY(EditAnywhere, Category = Gameplay)
	class UMaterialInterface* ImpactDecal;

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

public:
	ABaseWeapon2();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Shooting)
	class USoundBase* FireSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Shooting)
	class UAnimMontage* FireAnimation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Reloading)
	class USoundBase* ReloadSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Reloading)
	class UAnimMontage* ReloadAnimation;

	UPROPERTY(EditAnywhere, Category = Mesh)
	class USkeletalMeshComponent* FirearmMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 MaxReserveAmmo;

	UPROPERTY(BlueprintReadOnly)
	int32 ActualReserveAmmo;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 WeaponMagazinSize{30};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 AmmoCounter;

	UPROPERTY(EditAnywhere)
	float DamagePerShot{ 10.0f };

	UPROPERTY(EditAnywhere)
	float ShotDelay = 0.2f;

	UPROPERTY(EditAnywhere)
	float ReloadDelay = 1.2f;

	bool bIsFullAmmo = true;
	
	void Shoot();
	void Reload();
	void ReplenishAmmo();
};
