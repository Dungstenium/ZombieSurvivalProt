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
	//FirstPersonCameraComponent->RelativeLocation = ; // Position the camera
	FirstPersonCameraComponent->SetRelativeLocation(FVector(-39.56f, 1.75f, 64.f));
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	Mesh1P->SetOnlyOwnerSee(true);
	Mesh1P->SetupAttachment(FirstPersonCameraComponent);
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;
	Mesh1P->SetRelativeRotation(FRotator(1.9f, -19.19f, 5.2f));
	Mesh1P->SetRelativeLocation(FVector(-0.5f, -4.4f, -155.7f));
}

void AZombieSurvivalProtCharacter::BeginPlay()
{
	Super::BeginPlay();

	//Attach gun mesh component to Skeleton, doing it here because the skeleton is not yet created in the constructor
	FActorSpawnParameters SpawnParams;

	Rifle = GetWorld()->SpawnActor<ABaseWeapon2>(RifleBP, GetTransform(), SpawnParams);
	Pistol = GetWorld()->SpawnActor<ABaseWeapon2>(PistolBP, GetTransform(), SpawnParams);

	EquipedWeapon = Rifle;

	if (EquipedWeapon)
	{
		EquipedWeapon->FirearmMesh->AttachToComponent(Mesh1P, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("GripPoint"));
	}

	CurrentWeapon = ECurrentWeapon::Rifle;
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

	PlayerInputComponent->BindAction("Weapon1", IE_Pressed, this, &AZombieSurvivalProtCharacter::ChangeToWeapon1);
	PlayerInputComponent->BindAction("Weapon2", IE_Pressed, this, &AZombieSurvivalProtCharacter::ChangeToWeapon2);
	PlayerInputComponent->BindAction("Weapon3", IE_Pressed, this, &AZombieSurvivalProtCharacter::ChangeToWeapon3);
	PlayerInputComponent->BindAction("PreviousWeapon", IE_Pressed, this, &AZombieSurvivalProtCharacter::ChangeToPreviousWeapon);

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

void AZombieSurvivalProtCharacter::ChangeToWeapon1()
{
	if (EquipedWeapon != Rifle)
	{
		Rifle->SetActorRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
		EquipedWeapon = Rifle;
		EquipedWeapon->FirearmMesh->AttachToComponent(Mesh1P, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("GripPoint"));
		Pistol->SetActorRelativeLocation(FVector(-30.0f, 0.0f, 100.0f));
	}
}

void AZombieSurvivalProtCharacter::ChangeToWeapon2()
{
	if (EquipedWeapon != Pistol)
	{
		Pistol->SetActorRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
		EquipedWeapon = Pistol;
		EquipedWeapon->FirearmMesh->AttachToComponent(Mesh1P, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("GripPoint"));
		Rifle->SetActorRelativeLocation(FVector(-30.0f, 0.0f, 100.0f));
	}
}

void AZombieSurvivalProtCharacter::ChangeToWeapon3()
{

}

void AZombieSurvivalProtCharacter::ChangeToPreviousWeapon()
{

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

bool AZombieSurvivalProtCharacter::PressedInteractButton() const
{
	return bPlayerInteracted;
}

void AZombieSurvivalProtCharacter::OnFire() 
{
	if (PlayerAction == EPlayerAction::Idle)
	{
		EquipedWeapon->Shoot();
	}
	else
	{
		Reload();
	}
}

void AZombieSurvivalProtCharacter::Reload() 
{
	if (PlayerAction == EPlayerAction::Idle)
	{
		EquipedWeapon->Reload();
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

