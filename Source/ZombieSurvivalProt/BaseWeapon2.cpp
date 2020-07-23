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
#include "TimerManager.h" 
#include "ZombieSurvivalProtCharacter.h"


ABaseWeapon2::ABaseWeapon2()
{
	PrimaryActorTick.bCanEverTick = false;

	FirearmMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
	FirearmMesh->bCastDynamicShadow = false;
	FirearmMesh->CastShadow = false;
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
	if (!bIsClipEmpty)
	{
		Player->PlayerAction = EPlayerAction::Shooting;

		GetWorldTimerManager().SetTimer(WeaponTimerHandle, this, &ABaseWeapon2::WeaponCanShootAgain, ShotDelay, false);

		const FRotator SpawnRotation = GuideArrow->GetComponentRotation();
		const FVector SpawnLocation = GuideArrow->GetComponentLocation();
		FCollisionQueryParams TraceParams(FName(""), false, GetOwner());
		FHitResult Hit;

		FVector LineTraceEnd = SpawnLocation + SpawnRotation.Vector() * BulletRange;

		bool HitSomething = GetWorld()->LineTraceSingleByObjectType(
			OUT Hit,
			SpawnLocation,
			LineTraceEnd,
			FCollisionObjectQueryParams(ECollisionChannel::ECC_PhysicsBody),
			TraceParams);

		if (HitSomething)
		{
			AActor* ActorHit = Hit.GetActor();
			
			if (ActorHit)
			{
				UE_LOG(LogTemp, Warning, TEXT("Shot at: %s"), *ActorHit->GetName());
			
				if (ActorHit->GetClass()->IsChildOf(ASkeletalMeshActor::StaticClass()) && BulletImpactOnZombies)
				{
					//TODO: if hit a zombie, deal damage and spawn blood effect
					UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), BulletImpactOnZombies, Hit.Location, Hit.Normal.Rotation(), true);
				}
				else if (BulletImpactOnWalls && ImpactDecal)
				{
					UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), BulletImpactOnWalls, Hit.Location, Hit.Normal.Rotation(), true);

					FVector DecalSize = FVector(FMath::RandRange(7.0f, 12.0f));
					UGameplayStatics::SpawnDecalAttached(ImpactDecal,
						DecalSize,
						Hit.GetComponent(),
						NAME_None,
						Hit.Location, 
						Hit.Normal.Rotation(),
						EAttachLocation::KeepWorldPosition,
						60.0f);
				}
			}
		}

		SpawnSoundAndAnimate(SpawnLocation, FireSound, FireAnimation);
		SpawnMuzzleFlash(SpawnLocation, SpawnRotation);
		ReduceAmmoPerShot();

		DrawDebugLine(
			GetWorld(),
			SpawnLocation,
			LineTraceEnd,
			FColor::Red,
			false,
			10.2f,
			0,
			3.0f);
	}
	else
	{
		Reload();
	}
}

void ABaseWeapon2::ReduceAmmoPerShot()
{
	--AmmoCounter;

	if (bIsFullAmmo)
	{
		bIsFullAmmo = false;
	}
	else if (AmmoCounter <= 0)
	{
		bIsClipEmpty = true;
	}
}

void ABaseWeapon2::WeaponCanShootAgain()
{
	Player->PlayerAction = EPlayerAction::Idle;
	GetWorldTimerManager().ClearTimer(WeaponTimerHandle);
}

void ABaseWeapon2::SpawnSoundAndAnimate(const FVector& SpawnLocation, USoundBase* Sound, UAnimMontage* Animation)
{
	if (Sound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, Sound, SpawnLocation);
	}

	if (Animation && Player)
	{
		// Get the animation object for the arms mesh OF THE PLAYER
		UAnimInstance* AnimInstance = Player->GetMesh1P()->GetAnimInstance();
		if (AnimInstance)
		{
			AnimInstance->Montage_Play(Animation, 1.f);
		}
	}
}

void ABaseWeapon2::SpawnMuzzleFlash(const FVector& SpawnLocation, const FRotator& SpawnRotation)
{
	if (MuzzleFlash)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), MuzzleFlash, SpawnLocation, SpawnRotation, true);
	}
}

void ABaseWeapon2::Reload()
{
	if (ActualReserveAmmo > 0 && AmmoCounter != WeaponMagazinSize)
	{
		Player->PlayerAction = EPlayerAction::Reloading;

		GetWorldTimerManager().SetTimer(WeaponTimerHandle, this, &ABaseWeapon2::WeaponCanReloadAgain, ReloadDelay, false);

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

		SpawnSoundAndAnimate(GuideArrow->GetComponentLocation(), ReloadSound, ReloadAnimation);

		bIsClipEmpty = false;
	}
}

void ABaseWeapon2::WeaponCanReloadAgain()
{
	Player->PlayerAction = EPlayerAction::Idle;
	GetWorldTimerManager().ClearTimer(WeaponTimerHandle);
}

void ABaseWeapon2::ReplenishAmmo()
{
	if (!bIsFullAmmo)
	{
		ActualReserveAmmo = (WeaponMagazinSize - AmmoCounter) + MaxReserveAmmo;
		bIsFullAmmo = true;
	}
}
