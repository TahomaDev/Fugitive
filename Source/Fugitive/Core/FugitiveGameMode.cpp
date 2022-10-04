// Copyright Epic Games, Inc. All Rights Reserved.

#include "FugitiveGameMode.h"
#include "Fugitive/Player/FugitivePlayerController.h"
#include "UObject/ConstructorHelpers.h"

AFugitiveGameMode::AFugitiveGameMode()
{
	// use our custom PlayerController class
	PlayerControllerClass = AFugitivePlayerController::StaticClass();

	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("Blueprint'/Game/Game/Player/BP_Player.BP_Player_C'"));
	
	if (PlayerPawnBPClass.Class != nullptr)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}

