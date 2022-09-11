// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Engine/DataTable.h"
#include "Weapon.h"
#include "InventoryStructures.h"
#include "InventoryActor.h"
#include "InventoryComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSlotsUpdate);

USTRUCT(Blueprintable, BlueprintType)
struct FWeaponSlotPair : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int SlotIndex;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		AWeapon* Weapon;

	FWeaponSlotPair(unsigned int newSlotIndex, AWeapon* newWeapon) :
		SlotIndex(newSlotIndex),
		Weapon(newWeapon) {}

	FWeaponSlotPair() :
		SlotIndex(0),
		Weapon(nullptr) {}
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class LABGAMESSOFTWORKS_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UInventoryComponent();

	int GetSlotNums() { return SlotNums; }

	void UseSlot(int SlotIndex);

	UFUNCTION(BlueprintPure)
		FORCEINLINE TArray<FInventorySlot> GetSlots() { return Slots; }

	UFUNCTION(BlueprintPure)
		FORCEINLINE TArray<FWeaponSlotPair> GetWeapons() { return Weapons; }

	UFUNCTION(BlueprintPure)
		TArray<FWeaponState> GetWeaponsStates();

	UFUNCTION(BlueprintCallable)
		void SetSlots(TArray<FInventorySlot> slots, TMap<int,FWeaponState> weaponsStates);

	UFUNCTION(BlueprintPure)
		FORCEINLINE FInventorySlot GetSlotByIndex(int Index) { return Slots[Index]; }

	UFUNCTION(BlueprintPure)
		FORCEINLINE AWeapon* GetCurrentWeapon() { return CurrentWeapon; }

	UFUNCTION(BlueprintCallable)
		bool TryAddItem(FInventoryActorInfo ActorInfo);

	UFUNCTION(BlueprintCallable)
		bool TryRemoveItem(int SlotIndex);

	UPROPERTY(BlueprintAssignable, Category = "Delegates")
		FOnSlotsUpdate OnUpdateSlots;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	void CreateWeapon(AWeapon* Weapon, int slot);
	void CreateWeapon(FWeaponState WeaponState, int ID, int slot);
	
	void RemoveWeapon(AWeapon* Weapon);

private:

	unsigned int SlotNums;

	UPROPERTY(VisibleAnywhere, Category = "Slots")
		TArray<FInventorySlot> Slots;

	UPROPERTY(VisibleAnywhere, Category = "Stats")
		TSubclassOf<AInventoryActor> InventoryActorClass;

	UPROPERTY(VisibleAnywhere, Category = "Stats")
		AWeapon* CurrentWeapon;

	UPROPERTY(VisibleAnywhere, Category = "Stats")
		TArray<FWeaponSlotPair> Weapons;

};
