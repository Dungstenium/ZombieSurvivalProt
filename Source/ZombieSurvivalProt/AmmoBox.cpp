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
		if (Player->PlayerAction == EPlayerAction::Idle && Player->GetPlayerInteraction() && PlayerInReach && !Player->EquipedRifle->bIsFullAmmo)
		{
			Player->EquipedRifle->ReplenishAmmo();	
			Player->DeactivateInteractionWithObject();

			if (FireSound != NULL)
			{
				UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetActorLocation());
			}

			if (RearmAnimation != NULL && Player)
			{
				// Get the animation object for the arms mesh OF THE PLAYER
				UAnimInstance* AnimInstance = Player->GetMesh1P()->GetAnimInstance();
				if (AnimInstance != NULL)
				{
					AnimInstance->Montage_Play(RearmAnimation, 1.f);
				}
			}
			Timer += DeltaSeconds;
		}

		if (RearmAnimation != NULL)
		{
			if (Timer >= RearmAnimation->GetPlayLength())
			{
				Player->PlayerAction = EPlayerAction::Idle;
				Timer = 0.0f;
			}
			else if (Timer > 0)
			{
				if (Timer <= 0.3f)
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

	Trigger->OnComponentBeginOverlap.AddDynamic(this, &AAmmoBox::OnOverlapBegin);
	Trigger->OnComponentEndOverlap.AddDynamic(this, &AAmmoBox::OnOverlapEnd);
}

void AAmmoBox::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OnOverlapBegin(OverlappedComp, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);

	if (OtherActor->IsA<AZombieSurvivalProtCharacter>())
	{
		Player = Cast<AZombieSurvivalProtCharacter>(OtherActor);
		PlayerInReach = true;
		UE_LOG(LogTemp,Warning, TEXT("entered"))
	}
}

void AAmmoBox::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	Super::OnOverlapEnd(OverlappedComp, OtherActor, OtherComp, OtherBodyIndex);

	if (OtherActor->IsA<AZombieSurvivalProtCharacter>())
	{
		PlayerInReach = false;
		UE_LOG(LogTemp, Warning, TEXT("left"))
	}
}
