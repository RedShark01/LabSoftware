// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon.h"
#include "InventoryStructures.h"
#include "Engine/StreamableManager.h"
#include "Math/UnrealMathUtility.h"
#include "LabGameState.h"

// Sets default values
AWeapon::AWeapon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	WeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>("WeaponMesh");
	SetRootComponent(WeaponMesh);

	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AWeapon::AddAmmo(int add_Ammo)
{
	CurrentTotalAmmo = FMath::Clamp(CurrentTotalAmmo + add_Ammo, 0, MaxTotalAmmo);

	if (OnUpdateAmmo.IsBound())
		OnUpdateAmmo.Broadcast();
}

// Called when the game starts or when spawned
void AWeapon::BeginPlay()
{
	Super::BeginPlay();
}

void AWeapon::Reload()
{
	int NeedAmmo = MaxAmmo - CurrentAmmo;

	if (CurrentTotalAmmo >= NeedAmmo)
	{
		CurrentAmmo += NeedAmmo;
		CurrentTotalAmmo -= NeedAmmo;
	}
	else if (CurrentTotalAmmo > 0)
	{
		CurrentAmmo += CurrentTotalAmmo;
		CurrentTotalAmmo = 0;
	}

	if (OnUpdateAmmo.IsBound())
		OnUpdateAmmo.Broadcast();
}

// Called every frame
void AWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AWeapon::StartShoot()
{
	if (!CanShoot())
		return;

	if (isAuto)
	{
		Shoot();
		GetWorldTimerManager().SetTimer(ShootTimer, this, &AWeapon::Shoot, ShootRate, true);
	}
	else
		Shoot();
}

void AWeapon::StopShoot()
{
	if(isAuto && ShootTimer.IsValid())
		GetWorldTimerManager().ClearTimer(ShootTimer);
}

void AWeapon::Shoot()
{
	if (CanShoot())
	{
		FVector SpawnLocation = WeaponMesh->GetSocketLocation("Muzzle");
		FRotator SpawnRotation = WeaponMesh->GetSocketRotation("Muzzle");

		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = GetOwner();

		AProjectile* Projectile = GetWorld()->SpawnActor<AProjectile>(SpawnLocation, SpawnRotation, SpawnParams);
		Projectile->SetDamage(GetItemRowByIndex(WeaponID)->WeaponStats.Damage);

		CurrentAmmo--;

		if (OnUpdateAmmo.IsBound())
			OnUpdateAmmo.Broadcast();

		if (OnShoot.IsBound())
			OnShoot.Broadcast();
	}
	else if (isAuto)
		GetWorldTimerManager().ClearTimer(ShootTimer);
}

void AWeapon::Init(int weaponID, int newCurrentAmmo, int newCurrentTotalAmmo)
{
	if (weaponID == -1)
		return;

	FInventoryItemTableRow* ItemRow = GetItemRowByIndex(weaponID);

	if (!ItemRow)
		Destroy();

	WeaponID = weaponID;
	MaxAmmo = ItemRow->WeaponStats.MaxCurrentAmmo;
	MaxTotalAmmo = ItemRow->WeaponStats.MaxTotalAmmo;
	CurrentAmmo = newCurrentAmmo;
	CurrentTotalAmmo = newCurrentTotalAmmo;
	isAuto = ItemRow->WeaponStats.WeaponType == E_WeaponType::AutoRifle;
	ShootRate = ItemRow->WeaponStats.ShootRate;

	//Setup mesh with asyncLoad
	if (GetWorld() == nullptr)
		return;

	ALabGameState* GameState = Cast<ALabGameState>(GetWorld()->GetGameState());

	if (GameState == nullptr)
		return;

	FStreamableManager& AssetLoader = *GameState->AssetLoader;
	FSoftObjectPath Reference = ItemRow->ItemMesh.ToSoftObjectPath();

	AssetLoader.RequestAsyncLoad(Reference, [=]()
		{
			UStaticMesh* NewMesh = Cast<UStaticMesh>(StaticLoadObject(UStaticMesh::StaticClass(), nullptr, *(Reference.ToString())));
			WeaponMesh->SetStaticMesh(NewMesh);
		});

	if (OnUpdateAmmo.IsBound())
		OnUpdateAmmo.Broadcast();
}
