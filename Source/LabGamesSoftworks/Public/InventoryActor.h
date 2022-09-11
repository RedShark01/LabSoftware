// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "InventoryStructures.h"
#include "Engine/DataTable.h"
#include "Components/WidgetComponent.h"
#include "Components/SphereComponent.h"
#include "InventoryActor.generated.h"

UCLASS()
class LABGAMESSOFTWORKS_API AInventoryActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AInventoryActor();

	//Called on BeginPlay and on spawn from InventoryComponent
	void InitActorStats(FInventoryActorInfo Info);

	//Sets HealthBar visibility
	void SetWidgetVisibility(bool isVisible);

	//Getter for ItemInfo
	UFUNCTION(BlueprintPure)
	FORCEINLINE FInventoryActorInfo GetItemInfo() { return ItemInfo; }


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, Category = "Stats")
		FInventoryActorInfo ItemInfo;

	UPROPERTY(EditDefaultsOnly, Category = "Mesh")
		UStaticMeshComponent* ItemMesh;

	UPROPERTY(EditDefaultsOnly, Category = "Interact")
		USphereComponent* InteractRadiusSphere;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Interact")
		UWidgetComponent* InteractWidget;

	UFUNCTION(BlueprintImplementableEvent)
		void SetupWidgetParams();

};