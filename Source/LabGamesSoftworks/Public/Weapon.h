// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Projectile.h"
#include "Weapon.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnUpdateAmmo);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnShoot);

UCLASS()
class LABGAMESSOFTWORKS_API AWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWeapon();

	virtual void Init(int weaponID = -1, int newCurrentAmmo = 0, int newCurrentTotalAmmo = 0);

	UFUNCTION(BlueprintPure)
		FORCEINLINE int GetCurrentAmmo() { return CurrentAmmo; }
	
	UFUNCTION(BlueprintPure)
		FORCEINLINE int GetCurrentTotalAmmo() { return CurrentTotalAmmo; }

	UFUNCTION(BlueprintPure)
	FORCEINLINE int GetMaxTotalAmmo() { return MaxTotalAmmo; }
	
	UFUNCTION(BlueprintPure)
		FORCEINLINE int GetWeaponID() { return WeaponID; }

	UFUNCTION(BlueprintCallable)
		void AddAmmo(int add_Ammo);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	FTimerHandle ShootTimer;
	
	virtual void Shoot();
	virtual bool CanShoot() { return (CurrentAmmo > 0); }

	UPROPERTY(VisibleAnywhere, Category = "Stats")
		float ShootRate;

	UPROPERTY(VisibleAnywhere, Category = "Stats")
		int WeaponID;

	UPROPERTY(VisibleAnywhere, Category = "Stats")
		bool isAuto;

	UPROPERTY(VisibleAnywhere, Category = "State")
		int CurrentAmmo;

	UPROPERTY(VisibleAnywhere, Category = "Stats")
		int MaxAmmo;

	UPROPERTY(VisibleAnywhere, Category = "State")
		int CurrentTotalAmmo;

	UPROPERTY(VisibleAnywhere, Category = "State")
		int MaxTotalAmmo;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(BlueprintAssignable, Category = "Delegates")
		FOnUpdateAmmo OnUpdateAmmo;

	UPROPERTY(BlueprintAssignable, Category = "Delegates")
		FOnShoot OnShoot;

	void StartShoot();
	void StopShoot();

	void Reload();

protected:

	UPROPERTY(VisibleAnywhere)
		UStaticMeshComponent* WeaponMesh;
};
