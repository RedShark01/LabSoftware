// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "InventoryStructures.generated.h"

#define DATA_TABLE_PATH TEXT("DataTable'/Game/Blueprints/DT_InventoryItems.DT_InventoryItems'")

UENUM(BlueprintType)
enum class E_InventoryItemType : uint8
{
	Basic	UMETA(DisplayName = "Basic"),
	Weapon	UMETA(DisplayName = "Weapon"),
	Health	UMETA(DisplayName = "Health"),
	Ammo	UMETA(DisplayName = "Ammo"),
};

UENUM(BlueprintType)
enum class E_WeaponType : uint8
{
	AutoRifle	UMETA(DisplayName = "AutoRifle"),
	Pistol	UMETA(DisplayName = "Pistol"),
};

USTRUCT(Blueprintable)
struct FWeaponStats : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float Damage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float ShootRate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		E_WeaponType WeaponType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int MaxCurrentAmmo;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int MaxTotalAmmo;
};

USTRUCT(Blueprintable)
struct FWeaponState : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int CurrentAmmo;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int TotalAmmo;

	FWeaponState() :
		CurrentAmmo(0),
		TotalAmmo(0) {}

	FWeaponState(int newCurrentAmmo, int newTotalAmmo) :
		CurrentAmmo(newCurrentAmmo),
		TotalAmmo(newTotalAmmo) {}
};

USTRUCT(Blueprintable)
struct FInventoryItemTableRow : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int ItemId;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString ItemName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TSoftObjectPtr<UStaticMesh> ItemMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		E_InventoryItemType ItemType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool isUsable;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool CanCount;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FWeaponStats WeaponStats;

	FInventoryItemTableRow() :
		ItemId(0),
		ItemName(TEXT("")),
		ItemType(E_InventoryItemType::Basic),
		isUsable(false),
		WeaponStats()
	{}
};

USTRUCT(Blueprintable, BlueprintType)
struct FInventoryActorInfo : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int ItemID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int ItemCount;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FWeaponState WeaponInfo;

	FInventoryActorInfo() : ItemID(0), ItemCount(1), WeaponInfo(FWeaponState()) {}
	FInventoryActorInfo(int itemID, int itemCount, FWeaponState newWeaponInfo) :
		ItemID(itemID), 
		ItemCount(itemCount), 
		WeaponInfo(newWeaponInfo) {}

};

USTRUCT(Blueprintable, BlueprintType)
struct FInventorySlot : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int ItemID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int ItemCount;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		E_InventoryItemType ItemType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		E_WeaponType WeaponSlotType;

	
	bool isEmpty(){ return ItemID == -1; };

	FInventorySlot() : 
		ItemID(-1), 
		ItemCount(0), 
		ItemType(E_InventoryItemType::Basic) {}
	
	FInventorySlot(E_InventoryItemType newType) : 
		ItemID(-1), 
		ItemCount(0), 
		ItemType(newType),
		WeaponSlotType(E_WeaponType::AutoRifle) {}

	FInventorySlot(E_InventoryItemType newType, E_WeaponType newWeaponType) :
		ItemID(-1),
		ItemCount(0),
		ItemType(newType),
		WeaponSlotType(newWeaponType) {}

	FInventorySlot(int newID, unsigned int newItemCount, E_InventoryItemType newType, E_WeaponType newWeaponType) :
		ItemID(newID), 
		ItemCount(newItemCount), 
		ItemType(newType), 
		WeaponSlotType(newWeaponType) {}

};

UCLASS()
class UDataTable_Finder : public UObject
{
	GENERATED_BODY()

public:

	UDataTable_Finder();
	UDataTable* InventoryItemsDataTable;
};

UDataTable* GetInventoryDataTable();
FInventoryItemTableRow* GetItemRowByIndex(int Index);