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
	FName GetItemName() const { return ItemName; }

private:
	// Name of the item - used for lookup
	UPROPERTY(EditDefaultsOnly, Category = "Collectable")
	FName ItemName;
	
};
