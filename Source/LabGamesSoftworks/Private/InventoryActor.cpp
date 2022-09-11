// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryActor.h"
#include "Engine/StreamableManager.h"
#include "LabGameState.h"
#include "InventoryStructures.h"
#include "UObject/ConstructorHelpers.h"
#include "Engine/DataTable.h"

#define WIDGET_PATH TEXT("/Game/Widgets/W_InventoryActorWidget")

// Sets default values
AInventoryActor::AInventoryActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	//Set Overlap radius
	InteractRadiusSphere = CreateDefaultSubobject<USphereComponent>("SphereRadius");
	SetRootComponent(InteractRadiusSphere);

	InteractRadiusSphere->SetSphereRadius(100.f);

	//Setup ItemMesh component
	ItemMesh = CreateDefaultSubobject<UStaticMeshComponent>("ItemMesh");
	ItemMesh->SetupAttachment(RootComponent);

	//Setup Widget on Interact
	InteractWidget = CreateDefaultSubobject<UWidgetComponent>("InteractWidget");
	InteractWidget->SetupAttachment(RootComponent);

	InteractWidget->SetWidgetSpace(EWidgetSpace::Screen);
	InteractWidget->SetDrawSize(FVector2D(200.f, 100.f));
	InteractWidget->SetRelativeLocation(FVector(0.f, 50.f, 0.f));

	static ConstructorHelpers::FClassFinder<UUserWidget> UserWidget(WIDGET_PATH);
	
	if (UserWidget.Class)
		InteractWidget->SetWidgetClass(UserWidget.Class);
}

//Init stats and mesh
void AInventoryActor::InitActorStats(FInventoryActorInfo Info)
{
	if ((Info.ItemID == -1) || (Info.ItemCount <= 0))
		this->Destroy();

	ItemInfo = Info;

	FInventoryItemTableRow* ItemRow = GetItemRowByIndex(ItemInfo.ItemID);

	if (ItemRow == NULL)
		Destroy();

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
			ItemMesh->SetStaticMesh(NewMesh);
		});
}

// Called when the game starts or when spawned
void AInventoryActor::BeginPlay()
{
	Super::BeginPlay();

	InitActorStats(ItemInfo);
}

//Called on interact
void AInventoryActor::SetWidgetVisibility(bool isVisible)
{
	if (InteractWidget)
	{
		InteractWidget->SetVisibility(isVisible);
		SetupWidgetParams();
	}
}
