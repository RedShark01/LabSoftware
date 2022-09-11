// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "InventoryActor.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"

// Sets default values
APlayerCharacter::APlayerCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//Setup SpringArm component
	SpringArm = CreateDefaultSubobject<USpringArmComponent>("SpringArm");
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->TargetArmLength = 1000.f;
	SpringArm->SetRelativeRotation(FRotator(-60.f, 0.f, 0.f));

	//Setup Camera component
	PlayerCamera = CreateDefaultSubobject<UCameraComponent>("PlayerCamera");
	PlayerCamera->SetupAttachment(SpringArm);

	//Setup Health component
	HealthComponent = CreateDefaultSubobject<UHealthComponent>("HealthComponent");

	//Setup Inventory component
	InventoryComponent = CreateDefaultSubobject<UInventoryComponent>("InventoryComponent");

}

// Called when the game starts or when spawned
void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	//Bind delegates
	HealthComponent->OnTakeDamageEvent.BindUObject(this, &APlayerCharacter::OnTakeDamageEvent);
	HealthComponent->DeathEvent.BindUObject(this, &APlayerCharacter::OnDeathEvent);
}

// Called every frame
void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (HealthComponent->GetIsDead())
		return;

	//Each frame, the model rotates to face the cursor
	FRotator NewRotation = UKismetMathLibrary::FindLookAtRotation(GetMesh()->GetComponentLocation(), GetLocationUnderCursor());
	GetMesh()->SetRelativeRotation(FRotator(0.f, NewRotation.Yaw - 90.f,0.f));

	AInventoryActor* InteractActor = Cast<AInventoryActor>(GetActorUnderCursor());

	if (InteractActor)
	{
		if ((GetActorLocation() - InteractActor->GetActorLocation()).Size() <= 300)
		{
			if (InventoryActor)
			{
				if (InventoryActor != InteractActor)
				{
					InventoryActor->SetWidgetVisibility(false);
					InventoryActor = InteractActor;
					InventoryActor->SetWidgetVisibility(true);
				}
			}
			else
			{
				InventoryActor = InteractActor;
				InventoryActor->SetWidgetVisibility(true);
			}
		}
		else if(InventoryActor)
		{
			InventoryActor->SetWidgetVisibility(false);
			InventoryActor = nullptr;
		}
	}
	else if (InventoryActor)
	{
		InventoryActor->SetWidgetVisibility(false);
		InventoryActor = nullptr;
	}
}

// Called to bind functionality to input
void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAction("Movement", IE_Pressed, this, &APlayerCharacter::Movement);
	PlayerInputComponent->BindAction("Reload", IE_Pressed, this, &APlayerCharacter::Reload);

	PlayerInputComponent->BindAction("Shoot", IE_Pressed, this, &APlayerCharacter::StartShoot);
	PlayerInputComponent->BindAction("Shoot", IE_Released, this, &APlayerCharacter::StopShoot);

	PlayerInputComponent->BindAction("Interact", IE_Pressed, this, &APlayerCharacter::Interact);
	
	PlayerInputComponent->BindAction("SlotRemove", IE_Pressed, this, &APlayerCharacter::TryRemoveSlot);
	PlayerInputComponent->BindAction("UseSlot", IE_Pressed, this, &APlayerCharacter::TryUseSlot);
}

//Called by input key
void APlayerCharacter::Movement()
{
	APlayerController* PlayerController = Cast<APlayerController>(GetController());

	if (PlayerController)
		UAIBlueprintHelperLibrary::SimpleMoveToLocation(PlayerController, GetLocationUnderCursor());
}

void APlayerCharacter::Interact()
{
	if (InventoryActor)
	{
		if (InventoryComponent->TryAddItem(InventoryActor->GetItemInfo()))
			InventoryActor->Destroy();
	}
}

void APlayerCharacter::StartShoot()
{
	if (InventoryComponent->GetCurrentWeapon())
		InventoryComponent->GetCurrentWeapon()->StartShoot();
}

void APlayerCharacter::StopShoot()
{
	if (InventoryComponent->GetCurrentWeapon())
		InventoryComponent->GetCurrentWeapon()->StopShoot();
}

void APlayerCharacter::Reload()
{
	if (InventoryComponent->GetCurrentWeapon())
		InventoryComponent->GetCurrentWeapon()->Reload();
}

void APlayerCharacter::TryUseSlot(FKey Key)
{
	int SlotIndex = DigitalKeyToInt(Key) - 1;

	if (SlotIndex < InventoryComponent->GetSlotNums() && SlotIndex >= 0)
		InventoryComponent->UseSlot(SlotIndex);
}

void APlayerCharacter::TryRemoveSlot(FKey Key)
{
	int SlotIndex = DigitalKeyToInt(Key) - 1;
	
	if (SlotIndex < InventoryComponent->GetSlotNums() && SlotIndex >= 0)
		InventoryComponent->TryRemoveItem(SlotIndex);	
}

int APlayerCharacter::DigitalKeyToInt(FKey Key)
{
	FString KeyName = Key.ToString();

	if (Key.IsDigital())
	{
		if (KeyName == "One")
			return 1;
		if (KeyName == "Two")
			return 2;
		if (KeyName == "Three")
			return 3;
		if (KeyName == "Four")
			return 4;
		if (KeyName == "Five")
			return 5;
		if (KeyName == "Six")
			return 6;
		if (KeyName == "Seven")
			return 7;
		if (KeyName == "Eight")
			return 8;
		if (KeyName == "Nine")
			return 9;
	}
	
	return -1;
}

//Function to get the location under the cursor
FVector APlayerCharacter::GetLocationUnderCursor()
{
	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	
	FHitResult HitResult;

	if(PlayerController)
		PlayerController->GetHitResultUnderCursor(ECollisionChannel::ECC_Camera, false, HitResult);

	return HitResult.Location;
}

AActor* APlayerCharacter::GetActorUnderCursor()
{
	APlayerController* PlayerController = Cast<APlayerController>(GetController());

	FHitResult HitResult;
	
	if(PlayerController)
		PlayerController->GetHitResultUnderCursor(ECollisionChannel::ECC_Camera, false, HitResult);

	return HitResult.GetActor();
}

