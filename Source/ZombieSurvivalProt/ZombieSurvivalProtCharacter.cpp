// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "ZombieSurvivalProtCharacter.h"
#include "Animation/AnimInstance.h"
#include "BaseWeapon2.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "Components/TimelineComponent.h"
#include "GameFramework/CharacterMovementComponent.h" 
#include "GameFramework/InputSettings.h"
#include "LifeManager.h"
#include "UI_Interactor.h"
#include "ZombieSurvivalProtProjectile.h"

DEFINE_LOG_CATEGORY_STATIC(LogFPChar, Warning, All);

//////////////////////////////////////////////////////////////////////////
// AZombieSurvivalProtCharacter

AZombieSurvivalProtCharacter::AZombieSurvivalProtCharacter()
{
	TimeLine = CreateDefaultSubobject<UTimelineComponent>(TEXT("TimeLine"));

	InterpCrouchFunction.BindUFunction(this, FName("TimeLineFloatReturn"));
	TimeLineFinished.BindUFunction(this, FName("OnTimeLineFinished"));

	UI_Interactor = CreateDefaultSubobject<UUI_Interactor>(TEXT("UI Interactor"));
	LifeManager = CreateDefaultSubobject<ULifeManager>(TEXT("Life Manager"));

	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);

	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComponent->RelativeLocation = FVector(-39.56f, 1.75f, 64.f); // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	Mesh1P->SetOnlyOwnerSee(true);
	Mesh1P->SetupAttachment(FirstPersonCameraComponent);
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;
	Mesh1P->RelativeRotation = FRotator(1.9f, -19.19f, 5.2f);
	Mesh1P->RelativeLocation = FVector(-0.5f, -4.4f, -155.7f);
}

void AZombieSurvivalProtCharacter::BeginPlay()
{
	Super::BeginPlay();

	//Attach gun mesh component to Skeleton, doing it here because the skeleton is not yet created in the constructor
	FActorSpawnParameters SpawnParams;
	EquipedRifle = GetWorld()->SpawnActor<ABaseWeapon2>(RifleBP, GetTransform(), SpawnParams);

	if (EquipedRifle)
	{
		EquipedRifle->FirearmMesh->AttachToComponent(Mesh1P, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("GripPoint"));
	}

	ActiveWeapon = EActiveWeapon::Rifle;
	PlayerState = EPlayerMoveState::Idle;
	PlayerAction = EPlayerAction::Idle;

	if (TimelineCurve)
	{
		TimeLine->AddInterpFloat(TimelineCurve, InterpCrouchFunction, FName("Alpha"));

		TimeLine->SetTimelineFinishedFunc(TimeLineFinished);

		StandingHeight = 96.0f;
		CrouchedHeight = 30.0f;

		TimeLine->SetLooping(false);
	}
}

void AZombieSurvivalProtCharacter::TimeLineFloatReturn(float Value)
{
	if ((PlayerState == EPlayerMoveState::Crouching || (PlayerState != EPlayerMoveState::Crouching && GetCapsuleComponent()->GetScaledCapsuleHalfHeight() < StandingHeight)))
	{
		GetCapsuleComponent()->SetCapsuleHalfHeight(FMath::Lerp(StandingHeight, CrouchedHeight, Value));
		GetCharacterMovement()->MaxWalkSpeed = FMath::Lerp(MaxMoveSpeed, MaxCrouchedSpeed, Value);
	}
	else if(GetCapsuleComponent()->GetScaledCapsuleHalfHeight() == StandingHeight)
	{
		GetCapsuleComponent()->SetCapsuleHalfHeight(FMath::Lerp(GetCapsuleComponent()->GetScaledCapsuleHalfHeight(), StandingHeight, Value));
		GetCharacterMovement()->MaxWalkSpeed = FMath::Lerp(MaxMoveSpeed, 900.0f, Value);
	}
}

void AZombieSurvivalProtCharacter::OnTimeLineFinished()
{

}

void AZombieSurvivalProtCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// set up gameplay key bindings
	check(PlayerInputComponent);

	// Bind jump events
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	// Bind chrouch events
	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &AZombieSurvivalProtCharacter::PlayerCrouch);
	PlayerInputComponent->BindAction("Crouch", IE_Released, this, &AZombieSurvivalProtCharacter::PlayerUncrouch);

	// Bind run events
	PlayerInputComponent->BindAction("Run", IE_Pressed, this, &AZombieSurvivalProtCharacter::StartRunning);
	PlayerInputComponent->BindAction("Run", IE_Released, this, &AZombieSurvivalProtCharacter::StopRunning);

	// Bind fire event
	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &AZombieSurvivalProtCharacter::OnFire);

	PlayerInputComponent->BindAction("Reload", IE_Pressed, this, &AZombieSurvivalProtCharacter::Reload);

	PlayerInputComponent->BindAction("Interact", IE_Pressed, this, &AZombieSurvivalProtCharacter::InteractWithObject);
	PlayerInputComponent->BindAction("Interact", IE_Released, this, &AZombieSurvivalProtCharacter::DeactivateInteractionWithObject);

	PlayerInputComponent->BindAxis("MoveForward", this, &AZombieSurvivalProtCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AZombieSurvivalProtCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &AZombieSurvivalProtCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &AZombieSurvivalProtCharacter::LookUpAtRate);
}

void AZombieSurvivalProtCharacter::InteractWithObject()
{
	if (PlayerAction == EPlayerAction::Idle)
	{
		bPlayerInteracted = true;
	}
}

void AZombieSurvivalProtCharacter::DeactivateInteractionWithObject()
{
	bPlayerInteracted = false;
}

bool AZombieSurvivalProtCharacter::PlayerIsInteracting() const
{
	return bPlayerInteracted;
}

void AZombieSurvivalProtCharacter::OnFire()
{
	if (bHasAmmo && PlayerAction == EPlayerAction::Idle)
	{
		EquipedRifle->Shoot();
		PlayerAction = EPlayerAction::Shooting;
	}
	else if (!bHasAmmo && PlayerAction == EPlayerAction::Idle)
	{
		Reload();
	}
}

void AZombieSurvivalProtCharacter::Reload()
{
	if (PlayerAction == EPlayerAction::Idle)
	{
		EquipedRifle->Reload();
	}
}

void AZombieSurvivalProtCharacter::PlayerCrouch()
{
	PlayerState = EPlayerMoveState::Crouching;

	TimeLine->PlayFromStart();
}

void AZombieSurvivalProtCharacter::PlayerUncrouch()
{
	if (PlayerState != EPlayerMoveState::Running)
	{
		if (TimeLine->GetPlaybackPosition() <= 0.0f)
		{
			TimeLine->Play();
		}
		else
		{
			TimeLine->Reverse();
		}
		PlayerState = EPlayerMoveState::Idle;
	}
}

void AZombieSurvivalProtCharacter::StartRunning()
{
	PlayerState = EPlayerMoveState::Running;
	TimeLine->PlayFromStart();
}

void AZombieSurvivalProtCharacter::StopRunning()
{
	PlayerState = EPlayerMoveState::Idle;
	if (TimeLine->GetPlaybackPosition() <= 0.0f)
	{
		TimeLine->Play();
	}
	else
	{
		TimeLine->Reverse();
	}
}

void AZombieSurvivalProtCharacter::MoveForward(float Value)
{
	if (Value != 0.0f)
	{
		AddMovementInput(GetActorForwardVector(), Value);
	}
}

void AZombieSurvivalProtCharacter::MoveRight(float Value)
{
	if (Value != 0.0f)
	{
		AddMovementInput(GetActorRightVector(), Value);
	}
}

void AZombieSurvivalProtCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AZombieSurvivalProtCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

