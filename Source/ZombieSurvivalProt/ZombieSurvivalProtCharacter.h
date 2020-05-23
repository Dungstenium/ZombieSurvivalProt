// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Runtime/Engine/Classes/Components/TimelineComponent.h"
#include "ZombieSurvivalProtCharacter.generated.h"

UENUM()
enum class EActiveWeapon : uint8
{
	Unarmed,
	Pistol,
	Rifle
};

UENUM()
enum class EPlayerMoveState : uint8
{
	Idle,
	Crouching,
	Jumping,
	Running
};

UENUM()
enum class EPlayerAction : uint8
{
	Idle,
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

	/** First person camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FirstPersonCameraComponent;

	bool bHasAmmo = true;

	int32 EquipedMaxAmmo{ 30 };

	UPROPERTY(EditAnywhere)
	float BulletRange{ 3000.0f };

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
	int32 EquipedAmmoCounter{0};

	UPROPERTY(BlueprintReadOnly)
	int32 EquipedReserveAmmo{60};

	EActiveWeapon ActiveWeapon;
	EPlayerMoveState PlayerState;
	EPlayerAction PlayerAction;

	/** Returns Mesh1P subobject **/
	FORCEINLINE class USkeletalMeshComponent* GetMesh1P() const { return Mesh1P; }
	/** Returns FirstPersonCameraComponent subobject **/
	FORCEINLINE class UCameraComponent* GetFirstPersonCameraComponent() const { return FirstPersonCameraComponent; }

protected:
	
	virtual void BeginPlay();
		
	void OnFire();

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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (AllowPrivateAccess = true))
	class ABaseWeapon2* EquipedRifle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (AllowPrivateAccess = true))
	TSubclassOf<ABaseWeapon2> RifleBP;

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

	//Declare our delegate function to be binded with OnTimeLineFinished()
	FOnTimelineEvent TimeLineFinished{};

	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;

};

