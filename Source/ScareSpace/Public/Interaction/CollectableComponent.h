// Copyright Carter Wooton

#pragma once

#include "CoreMinimal.h"
#include "Interaction/InteractableComponent.h"
#include "CollectableComponent.generated.h"

/**
 * 
 */
UCLASS(ClassGroup = (Interactable), meta = (BlueprintSpawnableComponent))
class SCARESPACE_API UCollectableComponent : public UInteractableComponent
{
	GENERATED_BODY()

public:
	UCollectableComponent();

	// Called when interaction is set to begin
	virtual void BeginInteraction() override;


	UFUNCTION(BlueprintPure)
	FDataTableRowHandle GetItemRow() const { return CollectableItemRow; }

private:
	// Item row that this collectable represents. This is used to determine what item is being collected when the player interacts with it.
	UPROPERTY(EditDefaultsOnly, Category = "Collectable")
	FDataTableRowHandle CollectableItemRow;
	
};
