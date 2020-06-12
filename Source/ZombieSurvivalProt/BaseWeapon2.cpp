// Fill out your copyright notice in the Description page of Project Settings.

#include "BaseWeapon2.h"
#include "Animation/AnimInstance.h"
#include "Animation/SkeletalMeshActor.h"
#include "Components/ArrowComponent.h" 
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "Materials/MaterialInterface.h"
#include "Math/UnrealMathUtility.h"
#include "Particles/ParticleSystem.h"
#include "Public/CollisionQueryParams.h"
#include "ZombieSurvivalProtCharacter.h"


ABaseWeapon2::ABaseWeapon2()
{
	FirearmMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
	RootComponent = FirearmMesh;

	GuideArrow = CreateDefaultSubobject<UArrowComponent>(TEXT("Arrow"));
	GuideArrow->SetupAttachment(FirearmMesh);
	GuideArrow->bHiddenInGame = true;

	MuzzleFlash = CreateDefaultSubobject<UParticleSystem>(TEXT("WeaponMuzzleFlash"));

	BulletImpactOnWalls = CreateDefaultSubobject<UParticleSystem>(TEXT("BulletEffectlOnWalls"));
	BulletImpactOnZombies = CreateDefaultSubobject<UParticleSystem>(TEXT("BulletEffectlOnZombies"));

	ImpactDecal = CreateDefaultSubobject<UMaterialInterface>(TEXT("ShotDecal"));

	AmmoCounter = WeaponMagazinSize;
}

void ABaseWeapon2::BeginPlay()
{
	Super::BeginPlay();

	Player = Cast<AZombieSurvivalProtCharacter>(GetWorld()->GetFirstPlayerController()->GetCharacter());

	ActualReserveAmmo = MaxReserveAmmo;
}

void ABaseWeapon2::Shoot()
{
	if (bHasAmmo)
	{
		ReduceAmmoPerShot();

		const FRotator SpawnRotation = GuideArrow->GetComponentRotation();
		const FVector SpawnLocation = GuideArrow->GetComponentLocation();

		FVector LineTraceEnd = SpawnLocation + SpawnRotation.Vector() * BulletRange;

		DrawDebugLine(
			GetWorld(),
			SpawnLocation,
			LineTraceEnd,
			FColor::Red,
			false,
			10.2f,
			0,
			3.0f);

		FHitResult Hit;
		FCollisionQueryParams TraceParams(FName(""), false, GetOwner());
		GetWorld()->LineTraceSingleByObjectType(
			OUT Hit,
			SpawnLocation,
			LineTraceEnd,
			FCollisionObjectQueryParams(ECollisionChannel::ECC_PhysicsBody),
			TraceParams);

		AActor* ActorHit = Hit.GetActor();

		if (ActorHit)
		{
			UE_LOG(LogTemp, Warning, TEXT("Shot at: %s"), *ActorHit->GetName());
			
			if (ActorHit->GetClass()->IsChildOf(ASkeletalMeshActor::StaticClass()) && BulletImpactOnZombies)
			{
				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), BulletImpactOnZombies, Hit.Location, Hit.Normal.Rotation(), true);
			}
			else if (BulletImpactOnWalls && ImpactDecal)
			{
				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), BulletImpactOnWalls, Hit.Location, Hit.Normal.Rotation(), true);

				FVector DecalSize = FVector(FMath::RandRange(7.0f, 25.0f));
				UGameplayStatics::SpawnDecalAttached(ImpactDecal,
					DecalSize,
					Hit.GetComponent(),
					NAME_None, Hit.Location, 
					Hit.Normal.Rotation(),
					EAttachLocation::KeepWorldPosition,
					60.0f);
			}

		}

		if (FireSound != NULL)
		{
			UGameplayStatics::PlaySoundAtLocation(this, FireSound, GuideArrow->GetComponentLocation());
		}

		if (FireAnimation != NULL && Player)
		{
			// Get the animation object for the arms mesh OF THE PLAYER
			UAnimInstance* AnimInstance = Player->GetMesh1P()->GetAnimInstance();
			if (AnimInstance != NULL)
			{
				AnimInstance->Montage_Play(FireAnimation, 1.f);
			}
		}

		if (MuzzleFlash != NULL)
		{
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), MuzzleFlash, SpawnLocation, SpawnRotation, true);
		}
	}
	else
	{
		Reload();
	}
}

void ABaseWeapon2::ReduceAmmoPerShot()
{
	--AmmoCounter;

	if (AmmoCounter <= 0)
	{
		bHasAmmo = false;
	}
}

void ABaseWeapon2::Reload()
{
	if (ActualReserveAmmo > 0)
	{
		int32 AmmoDifference = WeaponMagazinSize - AmmoCounter;

		if (AmmoDifference >= ActualReserveAmmo)
		{
			AmmoCounter += ActualReserveAmmo;
			ActualReserveAmmo = 0;
		}
		else
		{
			AmmoCounter += AmmoDifference;
			ActualReserveAmmo -= AmmoDifference;
		}

		bHasAmmo = true;
	}
	else
	{

	}
}

void ABaseWeapon2::ReplenishAmmo()
{
	ActualReserveAmmo = (WeaponMagazinSize - AmmoCounter) + MaxReserveAmmo;
}
