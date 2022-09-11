// Fill out your copyright notice in the Description page of Project Settings.


#include "HealthComponent.h"
#include "HealthBarComponent.h"
#include "Math/UnrealMathUtility.h"

// Sets default values for this component's properties
UHealthComponent::UHealthComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame
	PrimaryComponentTick.bCanEverTick = false;

	//Setup default health
	DefaultHealth = 100.f;
	CurrentHealth = DefaultHealth;

	static ConstructorHelpers::FClassFinder<UHealthBarComponent> HealthBarClass(TEXT("/Game/Widgets/BP_HealthBarComponent"));

	if (HealthBarClass.Class)
		HealthBarSubclass = HealthBarClass.Class;
}

// Called when the game starts
void UHealthComponent::BeginPlay()
{
	Super::BeginPlay();

	if (GetOwner())
	{
		GetOwner()->OnTakeAnyDamage.AddDynamic(this, &UHealthComponent::OnTakeDamage);
		HealthBar = Cast<UHealthBarComponent>(GetOwner()->AddComponentByClass(HealthBarSubclass, false, FTransform(FVector(50.f,0.f,0.f)), false));
	}
}

//Post Init Properties
void UHealthComponent::PostInitProperties()
{
	Super::PostInitProperties();

	CalculateValues();
}

//Calculate Health value
void UHealthComponent::CalculateValues()
{
	CurrentHealth = DefaultHealth;
}

//Add current health
void UHealthComponent::HealthOwner(float addHealth)
{
	CurrentHealth = FMath::Clamp(CurrentHealth + addHealth, 0.f, DefaultHealth);
	HealthBar->SetHealth(CurrentHealth);
}

//Calls OnTakeDamage
void UHealthComponent::OnTakeDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser)
{
	if (isDead)
		return;

	CurrentHealth = FMath::Clamp(CurrentHealth - Damage, 0.f, DefaultHealth);

	if (CurrentHealth <= 0)
	{
		HealthBar->DestroyComponent();
		isDead = true;
		DeathEvent.ExecuteIfBound();
	}

	OnTakeDamageEvent.ExecuteIfBound(DamageCauser);
	HealthBar->SetHealth(CurrentHealth);
}