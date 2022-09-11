// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HealthBarComponent.h"
#include "HealthComponent.generated.h"

DECLARE_DELEGATE_OneParam(FOnTakeDamage, AActor*);
DECLARE_DELEGATE(FDeathEvent);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class LABGAMESSOFTWORKS_API UHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UHealthComponent();

	//Delegates
	FOnTakeDamage OnTakeDamageEvent;
	FDeathEvent DeathEvent;
	
	void HealthOwner(float addHealth);

	UFUNCTION(BlueprintPure)
		FORCEINLINE bool GetIsDead() { return isDead; }

	UFUNCTION(BlueprintCallable)
		FORCEINLINE void SetCurrentHealth(float newHealth) { CurrentHealth = newHealth; HealthBar->SetHealth(CurrentHealth); }

	FORCEINLINE bool CanHealth() { return (CurrentHealth < DefaultHealth) && !isDead; }

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	//PostInitProperties for values
	virtual void PostInitProperties() override;

	//Function for constructor
	virtual void CalculateValues();

	//On Take Damage
	UFUNCTION()
	void OnTakeDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser);

	//Health Bar component
	TSubclassOf<UHealthBarComponent> HealthBarSubclass;
	UHealthBarComponent* HealthBar;

	//Stats
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stats")
		float DefaultHealth;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State")
		float CurrentHealth;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State")
		bool isDead;

};