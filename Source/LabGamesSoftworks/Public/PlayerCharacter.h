// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "HealthComponent.h"
#include "InventoryActor.h"
#include "InventoryComponent.h"
#include "PlayerCharacter.generated.h"

UCLASS()
class LABGAMESSOFTWORKS_API APlayerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	APlayerCharacter();

	//Cosmetic events
	UFUNCTION(BlueprintImplementableEvent)
		void OnTakeDamageEvent(AActor* DamageCauser);

	UFUNCTION(BlueprintImplementableEvent)
		void OnDeathEvent();

	//Getters for components
	UFUNCTION(BlueprintPure)
		UInventoryComponent* GetInventoryComponent() { return InventoryComponent; }

	UFUNCTION(BlueprintPure)
		UHealthComponent* GetHealthComponent() { return HealthComponent; }

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	//Input Functions ---------------

	void Movement();
	void Interact();

	void StartShoot();
	void StopShoot();

	void Reload();

	void TryUseSlot(FKey Key);
	void TryRemoveSlot(FKey Key);

	//Key name to int for work with slots
	int DigitalKeyToInt(FKey Key);

	//Get location under cursor
	FVector GetLocationUnderCursor();
	AActor* GetActorUnderCursor();

	//Current inventory actor for interact
	AInventoryActor* InventoryActor;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:

	// Components ----------------

	UPROPERTY(EditDefaultsOnly, Category = "Camera")
		USpringArmComponent* SpringArm;

	UPROPERTY(EditDefaultsOnly, Category = "Camera")
		UCameraComponent* PlayerCamera;

	UPROPERTY(EditDefaultsOnly, Category = "Health")
		UHealthComponent* HealthComponent;

	UPROPERTY(EditDefaultsOnly, Category = "Inventory")
		UInventoryComponent* InventoryComponent;
};
