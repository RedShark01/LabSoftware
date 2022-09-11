// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "HealthComponent.h"
#include "Turret.generated.h"

UCLASS()
class LABGAMESSOFTWORKS_API ATurret : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATurret();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//Called every tick
	void SetRotation();
	void TraceSense();

	//Sets TargetPawn
	void SetTargetPawn(APawn* L_TargetPawn);
	void ClearTargetPawn();
	APawn* TargetPawn;

	//Shot
	void Shoot();
	FTimerHandle ShootTimer;

	//Delegates functions
	void OnTakeDamageEvent(AActor* DamageCauser);
	void OnDeathEvent();

	//Stats
	UPROPERTY(EditAnywhere, Category = "Stats")
		float TurretTurnSpeed;

	UPROPERTY(EditAnywhere, Category = "Stats")
		float TurretShootRate;

	UPROPERTY(EditAnywhere, Category = "Stats")
		float TurretDamage;

	UPROPERTY(EditAnywhere, Category = "Stats")
		float SenseRadius;

	//Components
	UPROPERTY(EditDefaultsOnly, Category = "Mesh")
		UStaticMeshComponent* TurretBase;

	UPROPERTY(EditDefaultsOnly, Category = "Mesh")
		UStaticMeshComponent* TurretHead;

	UPROPERTY(EditDefaultsOnly, Category = "Health")
		UHealthComponent* HealthComponent;

};
