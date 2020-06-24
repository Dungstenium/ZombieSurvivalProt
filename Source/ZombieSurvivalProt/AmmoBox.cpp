// Fill out your copyright notice in the Description page of Project Settings.


#include "AmmoBox.h"
#include "Animation/AnimInstance.h"
#include "Animation/SkeletalMeshActor.h"
#include "BaseWeapon2.h"
#include "Components/BoxComponent.h" 
#include "Components/InputComponent.h"
#include "Kismet/GameplayStatics.h"
#include "ZombieSurvivalProtCharacter.h"


AAmmoBox::AAmmoBox()
{
	PrimaryActorTick.bCanEverTick = true;

}

void AAmmoBox::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (Player)
	{
		if (Player->PlayerAction == EPlayerAction::Idle && Player->PlayerIsInteracting() && PlayerInReach && !Player->EquipedRifle->bIsFullAmmo)
		{
			Player->EquipedRifle->ReplenishAmmo();	
			Player->DeactivateInteractionWithObject();

			if (RearmSound)
			{
				UGameplayStatics::PlaySoundAtLocation(this, RearmSound, GetActorLocation());
			}

			if (RearmAnimation && Player)
			{
				// Get the animation object for the arms mesh OF THE PLAYER
				UAnimInstance* AnimInstance = Player->GetMesh1P()->GetAnimInstance();
				if (AnimInstance)
				{
					AnimInstance->Montage_Play(RearmAnimation, 1.f);
				}
			}
			Timer += DeltaSeconds;
		}

		if (RearmAnimation)
		{
			if (Timer >= RearmAnimation->GetPlayLength())
			{
				Player->PlayerAction = EPlayerAction::Idle;
				Timer = 0.0f;
				if (!PlayerInReach)
				{
					SetActorTickEnabled(false);
				}
			}
			else if (Timer > 0)
			{
				if (Timer <= 0.1f)
				{
					Player->PlayerAction = EPlayerAction::Interacting;
				}
				Timer += DeltaSeconds;
			}
		}
	}

}

void AAmmoBox::BeginPlay()
{
	Super::BeginPlay();

	SetActorTickEnabled(false);
}

void AAmmoBox::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OnOverlapBegin(OverlappedComp, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);

	if (OtherActor->IsA<AZombieSurvivalProtCharacter>())
	{
		Player = Cast<AZombieSurvivalProtCharacter>(OtherActor);
		PlayerInReach = true;
		SetActorTickEnabled(true);
	}
}

void AAmmoBox::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	Super::OnOverlapEnd(OverlappedComp, OtherActor, OtherComp, OtherBodyIndex);

	if (OtherActor->IsA<AZombieSurvivalProtCharacter>())
	{
		PlayerInReach = false;

		if (Player->PlayerAction == EPlayerAction::Idle)
		{
			SetActorTickEnabled(false);
		}
	}
}
