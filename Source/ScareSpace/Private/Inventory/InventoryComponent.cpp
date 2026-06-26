// Copyright Carter Wooton


#include "Inventory/InventoryComponent.h"
#include "GameFramework/Character.h"
#include "Controller/MainPlayerController.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Logging/ScareSpaceLogs.h"
#include "ItemStructs.h"
#include "Blueprint/UserWidget.h"

UInventoryComponent::UInventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}


void UInventoryComponent::ToggleInventoryMenu()
{
	if (!ThisController) return;

	bIsInventoryOpen = !bIsInventoryOpen;

	// Open the inventory menu
	if (bIsInventoryOpen)
	{
		// Create widget if not already created
		if (!IsValid(InventoryWidgetInstance) && InventoryWidgetClass)
		{
			UE_LOG(LogUI, Warning, TEXT("Inventory widget instance was not valid before opening"));
			InventoryWidgetInstance = CreateWidget<UUserWidget>(ThisController, InventoryWidgetClass);
		}
		if (IsValid(InventoryWidgetInstance))
		{
			// Show the UI
			InventoryWidgetInstance->SetIsFocusable(true);
			InventoryWidgetInstance->AddToViewport();
			// Tell Controller to use UI input mode
			ThisController->SetMenuState(true, InventoryWidgetInstance);
		}
	}
	// Close the inventory menu
	else
	{
		// Hide UI
		if (IsValid(InventoryWidgetInstance))
		{
			InventoryWidgetInstance->RemoveFromParent();
		}

		// Tell Controller to restore gameplay
		ThisController->SetMenuState(false, nullptr);
	}
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
				OnInventoryUpdated.Broadcast(InventoryItems);
				// List inventory items for debugging
				for (const FName& InventoryItem : InventoryItems)
				{
					UE_LOG(LogInventory, Display, TEXT("Current Inventory Item: %s"), *InventoryItem.ToString());
				}
				return true;
			}
			UE_LOG(LogInventory, Warning, TEXT("%s is already in the player's inventory"), *ItemName.ToString());
		}
		UE_LOG(LogInventory, Warning, TEXT("Item name: %s does not exist in the database"), *ItemName.ToString());
	}
	UE_LOG(LogInventory, Warning, TEXT("Item database is not found on the Inventory Component"));
	return false;
}

bool UInventoryComponent::RemoveItemFromInventory(FName ItemName)
{
	// only remove if item is in inventory
	if (InventoryItems.Contains(ItemName))
	{
		InventoryItems.Remove(ItemName);
		UE_LOG(LogInventory, Display, TEXT("Removed %s from inventory"), *ItemName.ToString());
		OnInventoryUpdated.Broadcast(InventoryItems);
		// List inventory items for debugging
		for (const FName& InventoryItem : InventoryItems)
		{
			UE_LOG(LogInventory, Display, TEXT("Current Inventory Item: %s"), *InventoryItem.ToString());
		}
		return true;
	}
	else
	{
		UE_LOG(LogInventory, Warning, TEXT("Cannot remove %s from inventory because it is not present"), *ItemName.ToString());
	}
	return false;
}

void UInventoryComponent::SelectItemByRowName(FName RowName)
{
	if (!IsValid(ItemDatabase))
	{
		return;
	}

	if (ItemDatabase->FindRow<FItemData>(RowName, TEXT("Selection Validation")))
	{
		CurrentSelectedRow = FDataTableRowHandle();
		CurrentSelectedRow.DataTable = ItemDatabase;
		CurrentSelectedRow.RowName = RowName;
		OnItemSelected.Broadcast(CurrentSelectedRow);
		ToggleInventoryMenu(); // Returns us back to the gameplay input mode, and closes the inventory menu
	}
}

bool UInventoryComponent::GetItemData(FName RowName, FItemData& OutItemData)
{
	if (!IsValid(ItemDatabase) || RowName.IsNone())
	{
		UE_LOG(LogInventory, Error, TEXT("Cannot get item data"));
		return false;
	}

	// Returns a direct pointer to the Data Table memory row
	if (FItemData* FoundItem = ItemDatabase->FindRow<FItemData>(RowName, TEXT("Item Data Lookup")))
	{
		OutItemData = *FoundItem;
		return true;
	}
	UE_LOG(LogInventory, Error, TEXT("Cannot get item data"));
	return false;
}

void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

	checkf(ItemDatabase, TEXT("ItemDatabase is not set on InventoryComponent of %s"), *GetOwner()->GetName());

	ACharacter* ThisChar = Cast<ACharacter>(GetOwner());
	checkf(ThisChar, TEXT("InventoryComponent must be attached to a Character!"));
	ThisController = Cast<AMainPlayerController>(ThisChar->GetController());
	checkf(ThisController, TEXT("InventoryComponent must be attached to a Character with a Player Controller!"));

	// Bind Interactor input actions to the controller
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
		ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(ThisController->GetLocalPlayer()))
	{
		Subsystem->AddMappingContext(InventoryGameplayContext, 0);
	}
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(ThisController->InputComponent))
	{
		// Interaction
		//EnhancedInputComponent->BindAction(OpenInventoryMenuAction, ETriggerEvent::Started, this, &UInventoryComponent::OpenInventoryMenu);
		//EnhancedInputComponent->BindAction(CloseInventoryMenuAction, ETriggerEvent::Started, this, &UInventoryComponent::CloseInventoryMenu);
		EnhancedInputComponent->BindAction(ToggleInventoryMenuAction, ETriggerEvent::Started, this, &UInventoryComponent::ToggleInventoryMenu);
	}

	// Create the inventory widget
	InventoryWidgetInstance = CreateWidget<UUserWidget>(GetWorld(), InventoryWidgetClass);

}

void UInventoryComponent::OpenInventoryMenu()
{	
	// Open inventory widget
	if (IsValid(InventoryWidgetInstance))
	{
		UE_LOG(LogInventory, Display, TEXT("Open Inventory"));

		checkf(ThisController, TEXT("Player Controller cannot be found when opening inventory."));
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(ThisController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(InventoryMenuContext, 10);
			Subsystem->RemoveMappingContext(InventoryGameplayContext);
		}

		InventoryWidgetInstance->AddToViewport();
		FInputModeGameAndUI InputModeData;
		InputModeData.SetWidgetToFocus(InventoryWidgetInstance->TakeWidget());
		ThisController->SetInputMode(InputModeData);
		ThisController->bShowMouseCursor = true;
		// Can I center cursor in UI?
		bIsInventoryOpen = true;
	}
}

void UInventoryComponent::CloseInventoryMenu()
{
	
	// Close inventory widget
	if (IsValid(InventoryWidgetInstance))
	{
		UE_LOG(LogInventory, Display, TEXT("Close Inventory"));
		checkf(ThisController, TEXT("Player Controller cannot be found when closing inventory."));
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(ThisController->GetLocalPlayer()))
		{
			Subsystem->RemoveMappingContext(InventoryMenuContext);
			Subsystem->AddMappingContext(InventoryGameplayContext, 0);
		}

		InventoryWidgetInstance->RemoveFromParent();
		FInputModeGameOnly InputModeData;
		ThisController->SetInputMode(InputModeData);
		ThisController->bShowMouseCursor = false;
		bIsInventoryOpen = false;
	}
}
