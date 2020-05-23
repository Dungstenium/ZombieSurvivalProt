// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseWeapon.h"
#include "Animation/AnimInstance.h"
#include "Components/ArrowComponent.h" 
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "Public/CollisionQueryParams.h"
#include "ZombieSurvivalProtCharacter.h"


UBaseWeapon::UBaseWeapon()
{
	PrimaryComponentTick.bCanEverTick = false;

	FirearmMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));

	GuideArrow = CreateDefaultSubobject<UArrowComponent>(TEXT("Arrow"));
	GuideArrow->SetupAttachment(FirearmMesh);
	GuideArrow->bHiddenInGame = true;

	AmmoCounter = MaxAmmo;
}

void UBaseWeapon::BeginPlay()
{
	Super::BeginPlay();

	Player = Cast<AZombieSurvivalProtCharacter>(GetWorld()->GetFirstPlayerController()->GetCharacter());
}

void UBaseWeapon::Shoot()
{
	if (bHasAmmo)
	{
		ReduceAmmoPerShot();

		const FRotator SpawnRotation = GuideArrow->GetComponentRotation();
		// MuzzleOffset is in camera space, so transform it to world space before offsetting from the character location to find the final muzzle position
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
			UE_LOG(LogTemp, Warning, TEXT("Shot at: %s"), *ActorHit->GetName())
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
	}
	else
	{
		Reload();
	}
}

void UBaseWeapon::ReduceAmmoPerShot()
{
	--AmmoCounter;

	if (AmmoCounter <= 0)
	{
		bHasAmmo = false;
	}
}

void UBaseWeapon::Reload()
{
	if (ReserveAmmo > 0)
	{
		int32 AmmoDifference = MaxAmmo - AmmoCounter;

		if (AmmoDifference >= ReserveAmmo)
		{
			AmmoCounter += ReserveAmmo;
			ReserveAmmo = 0;
		}
		else
		{
			AmmoCounter += AmmoDifference;
			ReserveAmmo -= AmmoDifference;
		}

		bHasAmmo = true;
	}
	else
	{

	}
}
