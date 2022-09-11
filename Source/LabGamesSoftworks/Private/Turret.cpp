// Fill out your copyright notice in the Description page of Project Settings.


#include "Turret.h"
#include "HealthComponent.h"
#include "Projectile.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
ATurret::ATurret()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//Setup static mesh - root
	TurretBase = CreateDefaultSubobject<UStaticMeshComponent>("TurretBase");
	SetRootComponent(TurretBase);

	//Setup static mesh - moveble turret head
	TurretHead = CreateDefaultSubobject<UStaticMeshComponent>("TurretHead");
	TurretHead->SetupAttachment(RootComponent);

	//Setup HealthComponent
	HealthComponent = CreateDefaultSubobject<UHealthComponent>("HealthComponent");

	//Default Turret stats
	TurretTurnSpeed = 1.f;
	TurretShootRate = 0.5f;
	TurretDamage = 10.f;
	SenseRadius = 1000.f;

}

// Called when the game starts or when spawned
void ATurret::BeginPlay()
{
	Super::BeginPlay();

	//Bind delegates
	HealthComponent->OnTakeDamageEvent.BindUObject(this, &ATurret::OnTakeDamageEvent);
	HealthComponent->DeathEvent.BindUObject(this, &ATurret::OnDeathEvent);
}

//Add rotation if haven`t target. If have target - look on target
void ATurret::SetRotation()
{
	if (TargetPawn)
	{
		FRotator Look = UKismetMathLibrary::FindLookAtRotation(TurretHead->GetComponentLocation(), TargetPawn->GetActorLocation());
		TurretHead->SetWorldRotation(FRotator(0.f, Look.Yaw - 90.f, 0.f));
	}
	else
		TurretHead->AddRelativeRotation(FRotator(0.f, TurretTurnSpeed, 0.f), true);
}

//Called every Tick. Make Sense for turret
void ATurret::TraceSense()
{
	FVector Start = TurretHead->GetComponentLocation() + FVector(0.f, 0.f, 50.f);
	FVector End = Start + TurretHead->GetRightVector() * SenseRadius;
	FVector BoxHalfSize = TargetPawn ? FVector(50.f, 50.f, 10.f) : FVector(100.f, 100.f, 10.f);

	static FHitResult HitRes;

	UKismetSystemLibrary::BoxTraceSingle(this, Start, End, BoxHalfSize, FRotator(0.f,0.f,0.f), ETraceTypeQuery::TraceTypeQuery1,
		false, { this }, EDrawDebugTrace::ForOneFrame, HitRes, true);

	APawn* SensePawn = Cast<APawn>(HitRes.GetActor());

	if(SensePawn)
		SetTargetPawn(SensePawn);
}

//Setter for TargetPawn
void ATurret::SetTargetPawn(APawn* L_TargetPawn)
{	
	if (TargetPawn && !L_TargetPawn)
	{
		GetWorldTimerManager().ClearTimer(ShootTimer);
		ShootTimer.Invalidate();
	}
	else if(!TargetPawn && L_TargetPawn)
		GetWorld()->GetTimerManager().SetTimer(ShootTimer, this, &ATurret::Shoot, TurretShootRate, true);

	TargetPawn = L_TargetPawn;

	static FTimerHandle Timer;
	
	if (Timer.IsValid())
		GetWorldTimerManager().ClearTimer(Timer);

	if(TargetPawn)
		GetWorldTimerManager().SetTimer(Timer, this, &ATurret::ClearTargetPawn, 1.f, false);
}

//Basic func for clear variable
void ATurret::ClearTargetPawn()
{
	SetTargetPawn(nullptr);
}

//Spawn projectile and set his damage
void ATurret::Shoot()
{
	static bool flipFlop = true;
	flipFlop = !flipFlop;

	const FVector ProjectileLocation = TurretHead->GetSocketLocation(flipFlop ? "Muzzle_1" : "Muzzle_2");
	const FRotator ProjectileRotation = TurretHead->GetSocketRotation(flipFlop ? "Muzzle_1" : "Muzzle_2");
	
	FActorSpawnParameters ProjectileSpawnPar;
	ProjectileSpawnPar.Owner = this;

	AProjectile* Projectile = GetWorld()->SpawnActor<AProjectile>(ProjectileLocation, ProjectileRotation, ProjectileSpawnPar);
	Projectile->SetDamage(TurretDamage);
}

//Calls On Take Damage from Health Component
void ATurret::OnTakeDamageEvent(AActor* DamageCauser)
{
	APawn* Attacker = Cast<APawn>(DamageCauser);

	if (Attacker)
		SetTargetPawn(Attacker);
}

// Called every frame
void ATurret::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	TraceSense();
	SetRotation();
}

// Calls on death from Health Component
void ATurret::OnDeathEvent()
{
	Destroy();
}

