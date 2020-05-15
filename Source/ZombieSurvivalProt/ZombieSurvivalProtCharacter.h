// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Runtime/Engine/Classes/Components/TimelineComponent.h"
#include "ZombieSurvivalProtCharacter.generated.h"

UENUM()
enum class EActiveWeapon : uint8
{
	Pistol,
	Rifle
};

UENUM()
enum class EPlayerState : uint8
{
	Idle,
	Crouching,
	Jumping,
	Running,
	Reloading,
	Interacting,
	Shooting
};

class UInputComponent;

UCLASS(config=Game)
class AZombieSurvivalProtCharacter : public ACharacter
{
	GENERATED_BODY()

private:

	/** Pawn mesh: 1st person view (arms; seen only by self) */
	UPROPERTY(VisibleDefaultsOnly, Category=Mesh)
	class USkeletalMeshComponent* Mesh1P;

	/** Gun mesh: 1st person view (seen only by self) */
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	class USkeletalMeshComponent* FP_Gun;

	/** Location on gun mesh where projectiles should spawn. */
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	class USceneComponent* FP_MuzzleLocation;

	/** First person camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FirstPersonCameraComponent;

	bool bHasAmmo = true;

	int32 MaxAmmo{ 30 };

public:
	
	AZombieSurvivalProtCharacter();
	
	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseLookUpRate;

	/** Gun muzzle's offset from the characters location */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Gameplay)
	FVector GunOffset;

	/** Sound to play each time we fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Gameplay)
	class USoundBase* FireSound;

	/** AnimMontage to play each time we fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	class UAnimMontage* FireAnimation;

	UPROPERTY(BlueprintReadOnly)
	int32 AmmoCounter{0};

	UPROPERTY(BlueprintReadOnly)
	int32 ReserveAmmo{60};

	EActiveWeapon ActiveWeapon;
	EPlayerState PlayerState;

protected:
	
	virtual void BeginPlay();
		
	void OnFire();

	void ReduceAmmoPerShot();

	void MoveForward(float Val);

	void MoveRight(float Val);

	/**
	 * Called via input to turn at a given rate.
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void TurnAtRate(float Rate);

	/**
	 * Called via input to turn look up/down at a given rate.
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void LookUpAtRate(float Rate);

	void PlayerCrouch();
	void PlayerUncrouch();

	void StartRunning();
	void StopRunning();

	void Reload();

	class UTimelineComponent* TimeLine;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class ULifeManager* LifeManager;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class UUI_Interactor* UI_Interactor;

	UPROPERTY(EditAnywhere)
		class UCurveFloat* TimelineCurve;

	UPROPERTY()
		float StandingHeight { 0 };

	UPROPERTY()
		float CrouchedHeight { 0 };

	UPROPERTY(EditAnywhere)
		float MaxMoveSpeed = 600.0f;

	UPROPERTY(EditAnywhere)
		float MaxCrouchedSpeed = 240.0f;

	UFUNCTION()
		void TimeLineFloatReturn(float Value);

	UFUNCTION()
		void OnTimeLineFinished();

	//Declare our delegate function to be binded with TimeLineFloatReturn
	FOnTimelineFloat InterpCrouchFunction{};

	FOnTimelineFloat InterpRunFunction{};

	//Declare our delegate function to be binded with OnTimeLineFinished()
	FOnTimelineEvent TimeLineFinished{};

	// APawn interface
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;
	// End of APawn interface

public:
	/** Returns Mesh1P subobject **/
	FORCEINLINE class USkeletalMeshComponent* GetMesh1P() const { return Mesh1P; }
	/** Returns FirstPersonCameraComponent subobject **/
	FORCEINLINE class UCameraComponent* GetFirstPersonCameraComponent() const { return FirstPersonCameraComponent; }

#pragma region TOUCHSTUFF

	private:
	/** Gun mesh: VR view (attached to the VR controller directly, no arm, just the actual gun) */
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	class USkeletalMeshComponent* VR_Gun;

	/** Location on VR gun mesh where projectiles should spawn. */
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	class USceneComponent* VR_MuzzleLocation;

	/** Motion controller (right hand) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UMotionControllerComponent* R_MotionController;

	/** Motion controller (left hand) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UMotionControllerComponent* L_MotionController;

	void BeginTouch(const ETouchIndex::Type FingerIndex, const FVector Location);
	void EndTouch(const ETouchIndex::Type FingerIndex, const FVector Location);
	void TouchUpdate(const ETouchIndex::Type FingerIndex, const FVector Location);

protected:

	/** Whether to use motion controller location for aiming. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	uint32 bUsingMotionControllers : 1;

	struct TouchData
	{
		TouchData() { bIsPressed = false;Location=FVector::ZeroVector;}
		bool bIsPressed;
		ETouchIndex::Type FingerIndex;
		FVector Location;
		bool bMoved;
	};
	TouchData	TouchItem;

	/* 
	 * Configures input for touchscreen devices if there is a valid touch interface for doing so 
	 *
	 * @param	InputComponent	The input component pointer to bind controls to
	 * @returns true if touch controls were enabled.
	 */
	bool EnableTouchscreenMovement(UInputComponent* InputComponent);

	/** Resets HMD orientation and position in VR. */
	void OnResetVR();

#pragma endregion
};

