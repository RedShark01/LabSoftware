// Fill out your copyright notice in the Description page of Project Settings.


#include "LabGameState.h"
#include "Engine/World.h"

ALabGameState::ALabGameState()
{
	AssetLoader = new FStreamableManager();
}
// Called when the game starts or when spawned
void ALabGameState::BeginPlay()
{
	Super::BeginPlay();
}
