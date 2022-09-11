// Copyright Epic Games, Inc. All Rights Reserved.


#include "LabGamesSoftworksGameModeBase.h"
#include "UObject/ConstructorHelpers.h"
#include "LabGameState.h"

#define PLAYER_PAWN_CLASS TEXT("/Game/Blueprints/BP_PlayerCharacter")
#define PLAYER_CONTROLLER_CLASS TEXT("/Game/Blueprints/BP_PlayerController")

//Set default classes for Gamemode
ALabGamesSoftworksGameModeBase::ALabGamesSoftworksGameModeBase()
{
	//Setup DefaultPawnClass
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(PLAYER_PAWN_CLASS);

	if (PlayerPawnBPClass.Class != NULL)
		DefaultPawnClass = PlayerPawnBPClass.Class;
	else
		UE_LOG(LogTemp, Warning, TEXT("Cant find PlayerPawnClass on path %s"), *PLAYER_PAWN_CLASS);

	//Setup PlayerControllerClass
	static ConstructorHelpers::FClassFinder<APlayerController> PlayerControllerBPClass(PLAYER_CONTROLLER_CLASS);

	if (PlayerControllerBPClass.Class != NULL)
		PlayerControllerClass = PlayerControllerBPClass.Class;
	else
		UE_LOG(LogTemp, Warning, TEXT("Cant find PlayerControllerClass on path %s"), *PLAYER_CONTROLLER_CLASS);

	GameStateClass = ALabGameState::StaticClass();

}