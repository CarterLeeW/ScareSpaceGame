// Copyright Carter Wooton

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ItemStructs.h"
#include "InventoryComponent.generated.h"

class UInputMappingContext;
class UInputAction;
class UUserWidget;
class AMainPlayerController;
struct FItemDataInventory;

// Define the event signature. It will send the complete set of item names.
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInventoryUpdatedSignature, const TArray<FDataTableRowHandle>&, NewItemSet);
// This event will be used for when an item is selected in the inventory widget. It will send the name of the selected item.
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnItemSelected, FDataTableRowHandle, SelectedItem);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SCARESPACE_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UInventoryComponent();

	/* Input */
	// Inventory Mapping Context during gameplay
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputMappingContext> InventoryGameplayContext;

	// Open Menu Input Action
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	TObjectPtr<UInputAction> OpenInventoryMenuAction;

	// Inventory Mapping Context during menus
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputMappingContext> InventoryMenuContext;

	// Close Menu Input Action
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	TObjectPtr<UInputAction> CloseInventoryMenuAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	TObjectPtr<UInputAction> ToggleInventoryMenuAction;

	// Toggle inventory: will replace open/close actions
	void ToggleInventoryMenu();

	/*
	* Operations within the inventory widget with mouse and keyboard will be handled within the widget itself
	* using widget events and not input actions.
	* On gameppad, there will be input actions for navigating the inventory UI.
	* These will be added later.
	*/

	/* End Input */

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool AddItemToInventory(FDataTableRowHandle CollectableItemRow);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool RemoveItemFromInventory(FDataTableRowHandle CollectableItemRow);

	// Returns current set of inventory items
	UFUNCTION(BlueprintPure, Category = "Inventory")
	TArray<FDataTableRowHandle> GetInventoryItems() const { return InventoryItems; }
	// This is the event the widget will bind to in order to update the inventory display
	UPROPERTY(BlueprintAssignable, Category = "Inventory|Events")
	FOnInventoryUpdatedSignature OnInventoryUpdated;

	// The controller that owns this interactor component
	TObjectPtr<AMainPlayerController> ThisController;

	// Allows communication with the interactor component and with the inventory widget
	UPROPERTY(BlueprintAssignable, Category = "Inventory|Events")
	FOnItemSelected OnItemSelected;

	// Call this when UI selection changes, passing the Row Name from your Inventory Table
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void SelectItemByRowName(FDataTableRowHandle CollectableItemRow);

	// Helper to let other systems fetch data safely
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool GetItemData(FDataTableRowHandle CollectableItemRow, UPARAM(ref) FItemDataInventory& OutItemData);

	// Item used and we can take it out of the inventory
	void ConsumeItem(FDataTableRowHandle CollectableItemRow);

protected:
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void OpenInventoryMenu();

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void CloseInventoryMenu();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory")
	TArray<FDataTableRowHandle> InventoryItems;

	//This is the main database of all items.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inventory")
	TObjectPtr<UDataTable> ItemDatabase;

	// Inventory Widget UMG Blueprint asset - must be set in the editor
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UUserWidget> InventoryWidgetClass;

	// Inventory widget instance
	UPROPERTY()
	TObjectPtr<UUserWidget> InventoryWidgetInstance;

	UPROPERTY(BlueprintReadOnly, Category = "Inventory")
	bool bIsInventoryOpen = false;

};
