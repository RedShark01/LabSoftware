// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Projectile.generated.h"

UCLASS()
class LABGAMESSOFTWORKS_API AProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AProjectile();

	//Sets damage
	FORCEINLINE void SetDamage(float nDamage) { Damage = nDamage; }

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	//Default damage on hit
	float Damage;

	// Called on hit
	UFUNCTION()
		void OnProjectileHit(UPrimitiveComponent* HitComp, AActor* OtherActor, 
			UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	//Components ----------------

	UPROPERTY(EditDefaultsOnly, Category = "Projectile")
		USphereComponent* ProjectileCollision;

	UPROPERTY(EditDefaultsOnly, Category = "Projectile")
		UStaticMeshComponent* ProjectileMesh;

	UPROPERTY(EditDefaultsOnly, Category = "Projectile")
		UProjectileMovementComponent* ProjectileMovement;

};
