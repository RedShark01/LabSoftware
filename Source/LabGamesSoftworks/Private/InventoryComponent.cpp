// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryComponent.h"
#include "InventoryStructures.h"
#include "InventoryActor.h"
#include "PlayerCharacter.h"
#include "UObject/ConstructorHelpers.h"

#define INVENTORY_ACTOR_PATH TEXT("/Game/BP_InventoryActor")

// Sets default values for this component's properties
UInventoryComponent::UInventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	static ConstructorHelpers::FClassFinder<AInventoryActor> InventoryActorClassFinder(INVENTORY_ACTOR_PATH);
	if (InventoryActorClassFinder.Class)
		InventoryActorClass = InventoryActorClassFinder.Class;
}

TArray<FWeaponState> UInventoryComponent::GetWeaponsStates()
{
	TArray<FWeaponState> WeaponStates = TArray<FWeaponState>();

	for (FWeaponSlotPair slot : Weapons)
		WeaponStates.Add(FWeaponState(slot.Weapon->GetCurrentAmmo(), slot.Weapon->GetMaxTotalAmmo()));
	
	return WeaponStates;
}

void UInventoryComponent::SetSlots(TArray<FInventorySlot> slots, TMap<int,FWeaponState> weaponsStates)
{
	Slots = slots;

	for (int x = 0; x < int(SlotNums); x++)
		if (Slots[x].ItemType == E_InventoryItemType::Weapon)
			Weapons.Add(FWeaponSlotPair(x, nullptr));

	for (auto& Elem : weaponsStates)
	{
		FInventorySlot CurrentSlot = Slots[Elem.Key];
		AWeapon* NewWeapon = NewObject<AWeapon>();

		NewWeapon->Init(CurrentSlot.ItemID, Elem.Value.CurrentAmmo, Elem.Value.TotalAmmo);

		for (int x = 0; x < Weapons.Num(); x++)
			if (Weapons[x].SlotIndex == Elem.Key)
				Weapons[x].Weapon = NewWeapon;
	}

	for (int x = 0; x < Weapons.Num(); x++)
		if (Weapons[x].Weapon != nullptr)
		{
			CreateWeapon(Weapons[x].Weapon, Weapons[x].SlotIndex);
			break;
		}

	if (OnUpdateSlots.IsBound())
		OnUpdateSlots.Broadcast();
}

bool UInventoryComponent::TryAddItem(FInventoryActorInfo ActorInfo)
{
	FInventoryItemTableRow* Row = GetItemRowByIndex(ActorInfo.ItemID);

	for (int x = 0; x < Slots.Num(); x++)
	{
		FInventorySlot* Slot = &Slots[x];

		if (Slot->ItemType != Row->ItemType)
			continue;

		if (Slot->isEmpty())
		{
			if (Slot->ItemType == E_InventoryItemType::Weapon)
			{
				if ((Slot->WeaponSlotType != Row->WeaponStats.WeaponType))
					continue;

				FWeaponStats WeaponStats = Row->WeaponStats;
						
				if (CurrentWeapon)
					RemoveWeapon(CurrentWeapon);

				CreateWeapon(ActorInfo.WeaponInfo, ActorInfo.ItemID, x);
			}
					
			Slots[x] = FInventorySlot(ActorInfo.ItemID, ActorInfo.ItemCount,
				Row->ItemType, Row->WeaponStats.WeaponType);

			if (OnUpdateSlots.IsBound())
				OnUpdateSlots.Broadcast();

			return true;
		}
		else if ((Slot->ItemID == Row->ItemId) && (Row->CanCount))
		{
			Slot->ItemCount += ActorInfo.ItemCount;

			if (OnUpdateSlots.IsBound())
				OnUpdateSlots.Broadcast();
			
			return true;
		}
		else
		{
			if (Row->ItemType == E_InventoryItemType::Weapon)
				if ((Slot->ItemType == E_InventoryItemType::Weapon) && (Slot->WeaponSlotType != Row->WeaponStats.WeaponType))
					continue;

			bool isHaveEmptySlot = false;
				
			for (int y = 0; y < Slots.Num(); y++)
				if ((Slots[y].ItemID == -1) && (Slots[y].ItemType == Slot->ItemType))
					isHaveEmptySlot = true;

			if (isHaveEmptySlot)
				continue;

			TryRemoveItem(x);

			if ((Slot->ItemType == E_InventoryItemType::Weapon) && (Slot->WeaponSlotType == Row->WeaponStats.WeaponType))
				CreateWeapon(ActorInfo.WeaponInfo, ActorInfo.ItemID, x);

			Slots[x] = FInventorySlot(ActorInfo.ItemID, ActorInfo.ItemCount, 
				Row->ItemType, Row->WeaponStats.WeaponType);

			if (OnUpdateSlots.IsBound())
				OnUpdateSlots.Broadcast();
			
			return true;
		}
	}

	return false;
}

bool UInventoryComponent::TryRemoveItem(int SlotIndex)
{
	if (Slots[SlotIndex].isEmpty())
		return false;

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
		
	FWeaponState WeaponState = FWeaponState();

	if (Slots[SlotIndex].ItemType == E_InventoryItemType::Weapon)
	{
		AWeapon* Weapon;

		for (int x = 0; x < Weapons.Num(); x++)
		{
			FWeaponSlotPair WSP = Weapons[x];

			if (WSP.SlotIndex == SlotIndex)
			{
				Weapon = WSP.Weapon;

				if (Weapon)
				{
					WeaponState = FWeaponState(Weapon->GetCurrentAmmo(), Weapon->GetCurrentTotalAmmo());

					if (Weapon == CurrentWeapon)
						RemoveWeapon(Weapon);
				}

				Weapons[x] = FWeaponSlotPair(SlotIndex, nullptr);

				if (CurrentWeapon == nullptr)
					for (int y = 0; y < Weapons.Num(); y++)
						if ((Weapons[y].Weapon != nullptr) && (Weapons[y].Weapon != Weapon))
							CreateWeapon(Weapons[y].Weapon, Weapons[y].SlotIndex);

				break;
			}
		}
	}

	AInventoryActor* InvActor =
		GetWorld()->SpawnActor<AInventoryActor>(InventoryActorClass, GetOwner()->GetActorLocation() + FVector(20.f, 20.f, -45.f), FRotator(), SpawnParams);
		
	if(InvActor)
		InvActor->InitActorStats(FInventoryActorInfo(Slots[SlotIndex].ItemID, Slots[SlotIndex].ItemCount, WeaponState));
		
	Slots[SlotIndex] = FInventorySlot(Slots[SlotIndex].ItemType,Slots[SlotIndex].WeaponSlotType);
		
	if (OnUpdateSlots.IsBound())
		OnUpdateSlots.Broadcast();

	return true;
}

void UInventoryComponent::UseSlot(int SlotIndex)
{
	FInventorySlot Slot = Slots[SlotIndex];
	FInventoryItemTableRow* ItemRow = GetItemRowByIndex(Slot.ItemID);

	if (!ItemRow)
		return;

	if (!ItemRow->isUsable && ItemRow->ItemType != E_InventoryItemType::Weapon)
		return;

	APlayerCharacter* Owner = Cast<APlayerCharacter>(GetOwner());
	AWeapon* Weapon = nullptr;

	switch (ItemRow->ItemType)
	{

	case E_InventoryItemType::Weapon:

		if (CurrentWeapon)
			if (CurrentWeapon->GetWeaponID() == Slot.ItemID)
				return;

		for (int x = 0; x < Weapons.Num(); x++)
			if (Weapons[x].SlotIndex == SlotIndex && Weapons[x].Weapon != nullptr)
				Weapon = Weapons[x].Weapon;

		if (Weapon)
		{
			RemoveWeapon(CurrentWeapon);
			CreateWeapon(Weapon, SlotIndex);
		}

		break;

	case E_InventoryItemType::Health:

		if (Owner)
		{
			if (Owner->GetHealthComponent()->CanHealth())
			{
				Owner->GetHealthComponent()->HealthOwner(20.f);

				if (ItemRow->CanCount && Slot.ItemCount > 0)
					--Slots[SlotIndex].ItemCount;

				if (Slots[SlotIndex].ItemCount <= 0)
					TryRemoveItem(SlotIndex);
			}
		}

		break;

	case E_InventoryItemType::Ammo:

		for (int x = 0; x < Slots.Num(); x++)
		{
			FInventoryItemTableRow* WeaponItemRow = GetItemRowByIndex(x);

			if (WeaponItemRow->ItemType == E_InventoryItemType::Weapon && (ItemRow->WeaponStats.WeaponType == WeaponItemRow->WeaponStats.WeaponType))
				for (FWeaponSlotPair WeaponSlot : Weapons)
					if (WeaponSlot.SlotIndex == x)
						Weapon = WeaponSlot.Weapon;

			if (Weapon)
				break;
		}

		if (Weapon)
		{
			int addAmmo = Weapon->GetMaxTotalAmmo() - Weapon->GetCurrentTotalAmmo();
			Weapon->AddAmmo((addAmmo > Slots[SlotIndex].ItemCount) ? Slots[SlotIndex].ItemCount : addAmmo);

			if (ItemRow->CanCount && Slot.ItemCount > 0)
				Slots[SlotIndex].ItemCount -= addAmmo;

			if (Slots[SlotIndex].ItemCount <= 0)
				TryRemoveItem(SlotIndex);
		}

		break;
	}

	if (OnUpdateSlots.IsBound())
		OnUpdateSlots.Broadcast();
}

// Called when the game starts
void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

	SlotNums = 5;

	Slots.Init(FInventorySlot(), SlotNums);

	Slots[0].ItemType = E_InventoryItemType::Weapon;
	Slots[0].WeaponSlotType = E_WeaponType::AutoRifle;

	Slots[1].ItemType = E_InventoryItemType::Weapon;
	Slots[1].WeaponSlotType = E_WeaponType::Pistol;

	Slots[2].ItemType = E_InventoryItemType::Health;
	Slots[3].ItemType = E_InventoryItemType::Ammo;
	Slots[4].ItemType = E_InventoryItemType::Ammo;

	for (int x = 0; x < int(SlotNums); x++)
		if (Slots[x].ItemType == E_InventoryItemType::Weapon)
			Weapons.Add(FWeaponSlotPair(x, nullptr));

}

//Create weapon mesh and sets current weapon
void UInventoryComponent::CreateWeapon(AWeapon* Weapon, int slot)
{
	if (!Weapon)
		return;

	ACharacter* Owner = Cast<ACharacter>(GetOwner());

	if (!Owner)
		return;

	int ID = Weapon->GetWeaponID();
	int CurrentAmmo = Weapon->GetCurrentAmmo();
	int CurrentTotalAmmo = Weapon->GetCurrentTotalAmmo();

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = Owner;

	Weapon = GetWorld()->SpawnActor<AWeapon>(SpawnParams);
	Weapon->Init(ID, CurrentAmmo, CurrentTotalAmmo);

	FAttachmentTransformRules Rules = FAttachmentTransformRules(EAttachmentRule::KeepRelative, false);
	Weapon->AttachToActor(Owner, Rules);
	Weapon->AttachToComponent(Owner->GetMesh(), Rules, FName("WeaponHand"));

	CurrentWeapon = Weapon;

	for (int y = 0; y < Weapons.Num(); y++)
		if (Weapons[y].SlotIndex == slot)
			Weapons[y].Weapon = CurrentWeapon;

}

//Create weapon mesh and sets current weapon
void UInventoryComponent::CreateWeapon(FWeaponState WeaponState, int ID, int slot)
{
	ACharacter* Owner = Cast<ACharacter>(GetOwner());
	
	if (!Owner)
		return;

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = Owner;
	
	AWeapon* Weapon = GetWorld()->SpawnActor<AWeapon>(SpawnParams);
	Weapon->Init(ID, WeaponState.CurrentAmmo, WeaponState.TotalAmmo);
	
	FAttachmentTransformRules Rules = FAttachmentTransformRules(EAttachmentRule::KeepRelative, false);
	Weapon->AttachToActor(Owner, Rules);
	Weapon->AttachToComponent(Owner->GetMesh(), Rules, FName("WeaponHand"));

	CurrentWeapon = Weapon;

	for (int y = 0; y < Weapons.Num(); y++)
		if (Weapons[y].SlotIndex == slot)
			Weapons[y].Weapon = CurrentWeapon;

}

//Remove weapon mesh
void UInventoryComponent::RemoveWeapon(AWeapon* Weapon)
{
	Weapon->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	Weapon->Destroy();
}