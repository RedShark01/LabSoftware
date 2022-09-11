// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "Engine/StreamableManager.h"
#include "LabGameState.generated.h"

UCLASS()
class LABGAMESSOFTWORKS_API ALabGameState : public AGameStateBase
{
	GENERATED_BODY()

public:

	ALabGameState();

	FStreamableManager* GetAssetLoader() { return AssetLoader; }

	FStreamableManager* AssetLoader;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;	
};

