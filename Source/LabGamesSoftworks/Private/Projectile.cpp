// Fill out your copyright notice in the Description page of Project Settings.


#include "Projectile.h"
#include "UObject/ConstructorHelpers.h"
#include "Kismet/GameplayStatics.h"

#define PROJECTILE_MESH_PATH TEXT("StaticMesh'/Engine/BasicShapes/Sphere.Sphere'")

// Sets default values
AProjectile::AProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	//Setup Root Collision
	ProjectileCollision = CreateDefaultSubobject<USphereComponent>("ProjectileCollision");
	SetRootComponent(ProjectileCollision);

	ProjectileCollision->SetSphereRadius(10.f);
	ProjectileCollision->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	ProjectileCollision->SetCollisionObjectType(ECollisionChannel::ECC_Visibility);
	ProjectileCollision->SetCollisionProfileName("InvisibleWall");

	//Setup Projectile Mesh
	ProjectileMesh = CreateDefaultSubobject<UStaticMeshComponent>("ProjectileMesh");
	ProjectileMesh->SetupAttachment(RootComponent);
	
	ProjectileMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	ProjectileMesh->SetWorldScale3D(FVector(0.15f));

	//Set mesh for Projectile from asset
	static ConstructorHelpers::FObjectFinder<UStaticMesh> ProjectileMeshObject(PROJECTILE_MESH_PATH);
	
	if (ProjectileMeshObject.Object)
		ProjectileMesh->SetStaticMesh(ProjectileMeshObject.Object);
	else
		UE_LOG(LogTemp, Warning, TEXT("Cant find ProjectileMeshObject on path %s"), *PROJECTILE_MESH_PATH);

	//Activate Projectile Movement
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>("ProjectileMovement");

	//ProjectileMovement setup
	ProjectileMovement->InitialSpeed = 3000.f;
	ProjectileMovement->MaxSpeed = 3000.f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->ProjectileGravityScale = 0.f;

	//Class Defaults
	Damage = 10.f;
}

// Called when the game starts or when spawned
void AProjectile::BeginPlay()
{
	Super::BeginPlay();

	//Bind OnHit event
	ProjectileCollision->OnComponentHit.AddDynamic(this, &AProjectile::OnProjectileHit);
	SetLifeSpan(10.f);
}

//calls with OnHitEvent 
void AProjectile::OnProjectileHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if(OtherActor)
		UGameplayStatics::ApplyDamage(OtherActor, Damage, GetInstigatorController(), GetOwner(), UDamageType::StaticClass());
	
	Destroy();
}

