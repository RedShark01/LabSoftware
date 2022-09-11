// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryStructures.h"
#include "UObject/ConstructorHelpers.h"

UDataTable_Finder::UDataTable_Finder()
{
	static ConstructorHelpers::FObjectFinder<UDataTable> InventoryItemsObject(DATA_TABLE_PATH);

	if (InventoryItemsObject.Object)
		InventoryItemsDataTable = InventoryItemsObject.Object;
}

UDataTable* GetInventoryDataTable()
{
	UDataTable_Finder* DT_Finder = NewObject<UDataTable_Finder>();

	if (DT_Finder)
	{
		UDataTable* DT = DT_Finder->InventoryItemsDataTable;
		return DT;
	}
	else
		return nullptr;
}

FInventoryItemTableRow* GetItemRowByIndex(int Index)
{
	TArray<FName> Names = GetInventoryDataTable()->GetRowNames();

	for (FName RowName : Names)
	{
		FInventoryItemTableRow* NewItem = GetInventoryDataTable()->FindRow<FInventoryItemTableRow>(RowName, "");

		if (NewItem->ItemId == Index)
			return NewItem;
	}

	return NULL;
}