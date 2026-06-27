// Copyright Carter Wooton


#include "Interaction/InteractableComponent.h"
#include "Logging/ScareSpaceLogs.h"

UInteractableComponent::UInteractableComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UInteractableComponent::BeginInteraction()
{
	//UE_LOG(LogTemp, Display, TEXT("Begin interaction Base Implementation"));
}

void UInteractableComponent::EndInteraction()
{
	//UE_LOG(LogTemp, Display, TEXT("End interaction Base Implementation"));
}

bool UInteractableComponent::QuickValidateItemInteraction(FDataTableRowHandle ItemRow)
{
	// No items can interact with this component
	if (InteractableItem.IsNull())
	{
		return false;
	}
	else if (InteractableItem == ItemRow)
	{
		return true;
	}
	return false;
}

// Need to make sure this returns true before taking item out of inventory
bool UInteractableComponent::TryInteractWithItem_Implementation(FDataTableRowHandle ItemRow)
{
	// Default behavior: check if the item matches the requirement
	if (!InteractableItem.IsNull() && ItemRow == InteractableItem)
	{
		// Return true so child Blueprints know the item matches
		return true;
	}
	return false;
}
