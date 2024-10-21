// Copyright Epic Games, Inc. All Rights Reserved.

#include "ScareSpaceGameMode.h"
#include "ScareSpaceCharacter.h"
#include "UObject/ConstructorHelpers.h"

AScareSpaceGameMode::AScareSpaceGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPerson/Blueprints/BP_FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

}
