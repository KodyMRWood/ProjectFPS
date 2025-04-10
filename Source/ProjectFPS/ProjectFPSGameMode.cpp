// Copyright Epic Games, Inc. All Rights Reserved.

#include "ProjectFPSGameMode.h"
#include "ProjectFPSCharacter.h"
#include "UObject/ConstructorHelpers.h"

AProjectFPSGameMode::AProjectFPSGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPerson/Blueprints/BP_FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

}
