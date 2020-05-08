// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "ZombieSurvivalProtCharacter.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "Components/TimelineComponent.h"
#include "GameFramework/CharacterMovementComponent.h" 
#include "GameFramework/InputSettings.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "LifeManager.h"
#include "MotionControllerComponent.h"
#include "UI_Interactor.h"
#include "XRMotionControllerBase.h" // for FXRMotionControllerBase::RightHandSourceId
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

	// Create a gun mesh component
	FP_Gun = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FP_Gun"));
	FP_Gun->SetOnlyOwnerSee(true);			// only the owning player will see this mesh
	FP_Gun->bCastDynamicShadow = false;
	FP_Gun->CastShadow = false;
	// FP_Gun->SetupAttachment(Mesh1P, TEXT("GripPoint"));
	FP_Gun->SetupAttachment(RootComponent);

	FP_MuzzleLocation = CreateDefaultSubobject<USceneComponent>(TEXT("MuzzleLocation"));
	FP_MuzzleLocation->SetupAttachment(FP_Gun);
	FP_MuzzleLocation->SetRelativeLocation(FVector(0.2f, 48.4f, -10.6f));

	// Default offset from the character location for projectiles to spawn
	GunOffset = FVector(100.0f, 0.0f, 10.0f);

	// Note: The ProjectileClass and the skeletal mesh/anim blueprints for Mesh1P, FP_Gun, and VR_Gun 
	// are set in the derived blueprint asset named MyCharacter to avoid direct content references in C++.

	// Create VR Controllers.
	R_MotionController = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("R_MotionController"));
	R_MotionController->MotionSource = FXRMotionControllerBase::RightHandSourceId;
	R_MotionController->SetupAttachment(RootComponent);
	L_MotionController = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("L_MotionController"));
	L_MotionController->SetupAttachment(RootComponent);

	// Create a gun and attach it to the right-hand VR controller.
	// Create a gun mesh component
	VR_Gun = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("VR_Gun"));
	VR_Gun->SetOnlyOwnerSee(true);			// only the owning player will see this mesh
	VR_Gun->bCastDynamicShadow = false;
	VR_Gun->CastShadow = false;
	VR_Gun->SetupAttachment(R_MotionController);
	VR_Gun->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));

	VR_MuzzleLocation = CreateDefaultSubobject<USceneComponent>(TEXT("VR_MuzzleLocation"));
	VR_MuzzleLocation->SetupAttachment(VR_Gun);
	VR_MuzzleLocation->SetRelativeLocation(FVector(0.000004, 53.999992, 10.000000));
	VR_MuzzleLocation->SetRelativeRotation(FRotator(0.0f, 90.0f, 0.0f));		// Counteract the rotation of the VR gun model.

	// Uncomment the following line to turn motion controllers on by default:
	//bUsingMotionControllers = true;
}

void AZombieSurvivalProtCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	//Attach gun mesh component to Skeleton, doing it here because the skeleton is not yet created in the constructor
	FP_Gun->AttachToComponent(Mesh1P, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("GripPoint"));

	// Show or hide the two versions of the gun based on whether or not we're using motion controllers.
	if (bUsingMotionControllers)
	{
		VR_Gun->SetHiddenInGame(false, true);
		Mesh1P->SetHiddenInGame(true, true);
	}
	else
	{
		VR_Gun->SetHiddenInGame(true, true);
		Mesh1P->SetHiddenInGame(false, true);
	}
	
	AmmoCounter = MaxAmmo;

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
	if ((bPlayerCrouched || (!bPlayerCrouched && GetCapsuleComponent()->GetScaledCapsuleHalfHeight() < StandingHeight)))
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

	// Enable touchscreen input
	EnableTouchscreenMovement(PlayerInputComponent);

	PlayerInputComponent->BindAction("ResetVR", IE_Pressed, this, &AZombieSurvivalProtCharacter::OnResetVR);

	// Bind movement events
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

void AZombieSurvivalProtCharacter::OnFire()
{
	if (bHasAmmo)
	{
		ReduceAmmoPerShot();

		// try and fire a projectile
		if (ProjectileClass != NULL)
		{
			UWorld* const World = GetWorld();
			if (World != NULL)
			{
				if (bUsingMotionControllers)
				{
					const FRotator SpawnRotation = VR_MuzzleLocation->GetComponentRotation();
					const FVector SpawnLocation = VR_MuzzleLocation->GetComponentLocation();
					World->SpawnActor<AZombieSurvivalProtProjectile>(ProjectileClass, SpawnLocation, SpawnRotation);
				}
				else
				{
					const FRotator SpawnRotation = GetControlRotation();
					// MuzzleOffset is in camera space, so transform it to world space before offsetting from the character location to find the final muzzle position
					const FVector SpawnLocation = ((FP_MuzzleLocation != nullptr) ? FP_MuzzleLocation->GetComponentLocation() : GetActorLocation()) + SpawnRotation.RotateVector(GunOffset);

					//Set Spawn Collision Handling Override
					FActorSpawnParameters ActorSpawnParams;
					ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;

					// spawn the projectile at the muzzle
					World->SpawnActor<AZombieSurvivalProtProjectile>(ProjectileClass, SpawnLocation, SpawnRotation, ActorSpawnParams);
				}
			}
		}

		// try and play the sound if specified
		if (FireSound != NULL)
		{
			UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetActorLocation());
		}

		// try and play a firing animation if specified
		if (FireAnimation != NULL)
		{
			// Get the animation object for the arms mesh
			UAnimInstance* AnimInstance = Mesh1P->GetAnimInstance();
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

void AZombieSurvivalProtCharacter::ReduceAmmoPerShot()
{
	--AmmoCounter;

	UE_LOG(LogTemp, Warning, TEXT("ActualAmmo: %i ReserveAmmo: %i"), AmmoCounter, ReserveAmmo);

	if (AmmoCounter <= 0)
	{
		bHasAmmo = false;
	}
}

void AZombieSurvivalProtCharacter::Reload()
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
		UE_LOG(LogTemp, Warning, TEXT("REALOADED: ActualAmmo: %i ReserveAmmo: %i"), AmmoCounter, ReserveAmmo);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("OUT OF AMMO"));
	}
}

void AZombieSurvivalProtCharacter::OnResetVR()
{
	UHeadMountedDisplayFunctionLibrary::ResetOrientationAndPosition();
}

void AZombieSurvivalProtCharacter::PlayerCrouch()
{
	bPlayerCrouched = true;

	TimeLine->PlayFromStart();
}

void AZombieSurvivalProtCharacter::PlayerUncrouch()
{
	if (!bPlayerRunning)
	{
		if (TimeLine->GetPlaybackPosition() <= 0.0f)
		{
			TimeLine->Play();
		}
		else
		{
			TimeLine->Reverse();
		}
	}

	bPlayerCrouched = false;
}

void AZombieSurvivalProtCharacter::StartRunning()
{
	bPlayerRunning = true;
	TimeLine->PlayFromStart();
}

void AZombieSurvivalProtCharacter::StopRunning()
{
	if (TimeLine->GetPlaybackPosition() <= 0.0f)
	{
		TimeLine->Play();
	}
	else
	{
		TimeLine->Reverse();
	}

	bPlayerRunning = false;
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

#pragma region TOUCHSTUFF

bool AZombieSurvivalProtCharacter::EnableTouchscreenMovement(class UInputComponent* PlayerInputComponent)
{
	if (FPlatformMisc::SupportsTouchInput() || GetDefault<UInputSettings>()->bUseMouseForTouch)
	{
		PlayerInputComponent->BindTouch(EInputEvent::IE_Pressed, this, &AZombieSurvivalProtCharacter::BeginTouch);
		PlayerInputComponent->BindTouch(EInputEvent::IE_Released, this, &AZombieSurvivalProtCharacter::EndTouch);

		//Commenting this out to be more consistent with FPS BP template.
		//PlayerInputComponent->BindTouch(EInputEvent::IE_Repeat, this, &AZombieSurvivalProtCharacter::TouchUpdate);
		return true;
	}
	
	return false;
}

void AZombieSurvivalProtCharacter::BeginTouch(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	if (TouchItem.bIsPressed == true)
	{
		return;
	}
	if ((FingerIndex == TouchItem.FingerIndex) && (TouchItem.bMoved == false))
	{
		OnFire();
	}
	TouchItem.bIsPressed = true;
	TouchItem.FingerIndex = FingerIndex;
	TouchItem.Location = Location;
	TouchItem.bMoved = false;
}

void AZombieSurvivalProtCharacter::EndTouch(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	if (TouchItem.bIsPressed == false)
	{
		return;
	}
	TouchItem.bIsPressed = false;
}

//void AZombieSurvivalProtCharacter::TouchUpdate(const ETouchIndex::Type FingerIndex, const FVector Location)
//{
//	if ((TouchItem.bIsPressed == true) && (TouchItem.FingerIndex == FingerIndex))
//	{
//		if (TouchItem.bIsPressed)
//		{
//			if (GetWorld() != nullptr)
//			{
//				UGameViewportClient* ViewportClient = GetWorld()->GetGameViewport();
//				if (ViewportClient != nullptr)
//				{
//					FVector MoveDelta = Location - TouchItem.Location;
//					FVector2D ScreenSize;
//					ViewportClient->GetViewportSize(ScreenSize);
//					FVector2D ScaledDelta = FVector2D(MoveDelta.X, MoveDelta.Y) / ScreenSize;
//					if (FMath::Abs(ScaledDelta.X) >= 4.0 / ScreenSize.X)
//					{
//						TouchItem.bMoved = true;
//						float Value = ScaledDelta.X * BaseTurnRate;
//						AddControllerYawInput(Value);
//					}
//					if (FMath::Abs(ScaledDelta.Y) >= 4.0 / ScreenSize.Y)
//					{
//						TouchItem.bMoved = true;
//						float Value = ScaledDelta.Y * BaseTurnRate;
//						AddControllerPitchInput(Value);
//					}
//					TouchItem.Location = Location;
//				}
//				TouchItem.Location = Location;
//			}
//		}
//	}
//}

//Commenting this section out to be consistent with FPS BP template.
//This allows the user to turn without using the right virtual joystick
#pragma endregion
