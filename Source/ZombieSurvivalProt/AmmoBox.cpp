// Fill out your copyright notice in the Description page of Project Settings.


#include "AmmoBox.h"
#include "Animation/AnimInstance.h"
#include "Animation/SkeletalMeshActor.h"
#include "BaseWeapon2.h"
#include "Components/BoxComponent.h" 
#include "Components/InputComponent.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h" 
#include "ZombieSurvivalProtCharacter.h"


AAmmoBox::AAmmoBox()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AAmmoBox::BeginPlay()
{
	Super::BeginPlay();

	SetActorTickEnabled(false);
}

void AAmmoBox::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (Player)
	{
		if (Player->PlayerAction == EPlayerAction::Idle && Player->PressedInteractButton() && PlayerInReach && !Player->EquipedWeapon->bIsFullAmmo)
		{
			Player->EquipedWeapon->ReplenishAmmo();	
			Player->DeactivateInteractionWithObject();
			Player->PlayerAction = EPlayerAction::Interacting;

			GetWorld()->GetTimerManager().SetTimer(AmmoBoxTimerHandle, this, &AAmmoBox::FinishRearming, RearmAnimation->GetPlayLength(), false);

			if (RearmSound)
			{
				UGameplayStatics::PlaySoundAtLocation(this, RearmSound, GetActorLocation());
			}

			if (RearmAnimation)
			{
				// Get the animation object for the arms mesh of the player
				UAnimInstance* AnimInstance = Player->GetMesh1P()->GetAnimInstance();
				if (AnimInstance)
				{
					AnimInstance->Montage_Play(RearmAnimation, 1.f);
				}
			}
		}
	}
}

void AAmmoBox::FinishRearming()
{
	Player->PlayerAction = EPlayerAction::Idle;

	// If player finished reloading and left the triggerbox. Then deactivate Tick
	if (!PlayerInReach)
	{
		SetActorTickEnabled(false);
	}
}

void AAmmoBox::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OnOverlapBegin(OverlappedComp, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);

	if (OtherActor->IsA<AZombieSurvivalProtCharacter>())
	{
		if (!Player)
		{
			Player = Cast<AZombieSurvivalProtCharacter>(OtherActor);
		}

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
