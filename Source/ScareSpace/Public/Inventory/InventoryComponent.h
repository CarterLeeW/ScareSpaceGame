// Copyright Carter Wooton

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InventoryComponent.generated.h"

class UInputMappingContext;
class UInputAction;

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

	/*
	* Operations within the inventory widget with mouse and keyboard will be handled within the widget itself
	* using widget events and not input actions.
	* On gameppad, there will be input actions for navigating the inventory UI.
	* These will be added later.
	*/

	/* End Input */

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool AddItemToInventory(FName ItemName);

	// The controller that owns this interactor component
	TObjectPtr<APlayerController> ThisController;

protected:
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void OpenInventoryMenu();

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void CloseInventoryMenu();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory")
	TSet<FName> InventoryItems;

	//This is the main database of all items.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inventory")
	TObjectPtr<UDataTable> ItemDatabase;

	UPROPERTY(BlueprintReadOnly, Category = "Inventory")
	bool bIsInventoryOpen = false;
};
