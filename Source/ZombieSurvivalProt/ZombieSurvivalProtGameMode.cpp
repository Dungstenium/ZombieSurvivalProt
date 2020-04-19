// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "ZombieSurvivalProtGameMode.h"
#include "ZombieSurvivalProtHUD.h"
#include "ZombieSurvivalProtCharacter.h"
#include "UObject/ConstructorHelpers.h"

AZombieSurvivalProtGameMode::AZombieSurvivalProtGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPersonCPP/Blueprints/FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

	// use our custom HUD class
	HUDClass = AZombieSurvivalProtHUD::StaticClass();
}
