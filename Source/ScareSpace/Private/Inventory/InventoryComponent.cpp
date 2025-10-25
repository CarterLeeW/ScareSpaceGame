// Copyright Carter Wooton


#include "Inventory/InventoryComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/PlayerController.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Logging/ScareSpaceLogs.h"
#include "ItemStructs.h"
UInventoryComponent::UInventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}


bool UInventoryComponent::AddItemToInventory(FName ItemName)
{
	// there are no duplicate items in the game
	// all are key items
	if (ItemDatabase)
	{
		// does item exist in database?
		if (FItemData* FoundItem = ItemDatabase->FindRow<FItemData>(ItemName, TEXT("Looking up item in database")))
		{
			// is item not already in our inventory? Should never be false since no duplicates allowed
			if (!InventoryItems.Contains(ItemName))
			{
				InventoryItems.Add(ItemName);
				UE_LOG(LogInventory, Display, TEXT("Adding %s to inventory"), *ItemName.ToString());
				return true;
			}
			UE_LOG(LogInventory, Error, TEXT("%s is already in the player's inventory"), *ItemName.ToString());
		}
		UE_LOG(LogInventory, Error, TEXT("Item name: %s does not exist in the database"), *ItemName.ToString());
	}
	UE_LOG(LogInventory, Error, TEXT("Item database is not found on the Inventory Component"));
	return false;
}

void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

	checkf(ItemDatabase, TEXT("ItemDatabase is not set on InventoryComponent of %s"), *GetOwner()->GetName());

	ACharacter* ThisChar = Cast<ACharacter>(GetOwner());
	checkf(ThisChar, TEXT("InventoryComponent must be attached to a Character!"));
	ThisController = Cast<APlayerController>(ThisChar->GetController());
	checkf(ThisController, TEXT("InventoryComponent must be attached to a Character with a PlayerController!"));

	// Bind Interactor input actions to the controller
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
		ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(ThisController->GetLocalPlayer()))
	{
		Subsystem->AddMappingContext(InventoryGameplayContext, 0);
	}
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(ThisController->InputComponent))
	{
		// Interaction
		EnhancedInputComponent->BindAction(OpenInventoryMenuAction, ETriggerEvent::Started, this, &UInventoryComponent::OpenInventoryMenu);
		EnhancedInputComponent->BindAction(CloseInventoryMenuAction, ETriggerEvent::Started, this, &UInventoryComponent::CloseInventoryMenu);
	}

}

void UInventoryComponent::OpenInventoryMenu()
{
	UE_LOG(LogInventory, Display, TEXT("Open Inventory"));

	checkf(ThisController, TEXT("PlayerController cannot be found when opening inventory."));
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(ThisController->GetLocalPlayer()))
	{
		Subsystem->AddMappingContext(InventoryMenuContext, 0);
		Subsystem->RemoveMappingContext(InventoryGameplayContext);
	}
	bIsInventoryOpen = true;
}

void UInventoryComponent::CloseInventoryMenu()
{
	UE_LOG(LogInventory, Display, TEXT("Close Inventory"));
	checkf(ThisController, TEXT("PlayerController cannot be found when closing inventory."));
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(ThisController->GetLocalPlayer()))
	{
		Subsystem->RemoveMappingContext(InventoryMenuContext);
		Subsystem->AddMappingContext(InventoryGameplayContext, 0);
	}
	bIsInventoryOpen = false;
}
