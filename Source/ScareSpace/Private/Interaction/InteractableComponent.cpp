// Copyright Carter Wooton


#include "Interaction/InteractableComponent.h"
#include "Logging/ScareSpaceLogs.h"

UInteractableComponent::UInteractableComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UInteractableComponent::BeginInteraction()
{
	InteractionCounter = FMath::Clamp(InteractionCounter + 1, 0, 255);
	OnInteractionBegins.Broadcast();
	//UE_LOG(LogTemp, Display, TEXT("Begin interaction Base Implementation"));
}

void UInteractableComponent::EndInteraction()
{
	//UE_LOG(LogTemp, Display, TEXT("End interaction Base Implementation"));
}

bool UInteractableComponent::QuickValidateItemInteraction(FDataTableRowHandle CollectableItemRow)
{
	// No items can interact with this component
	if (InteractableItem.IsNull())
	{
		return false;
	}
	else if (InteractableItem == CollectableItemRow)
	{
		return true;
	}
	return false;
}

// Need to make sure this returns true before taking item out of inventory
bool UInteractableComponent::TryInteractWithItem(FDataTableRowHandle CollectableItemRow)
{
	// Default behavior: check if the item matches the requirement
	if (!InteractableItem.IsNull() && CollectableItemRow == InteractableItem)
	{
		// Do stuff like unlocking
		OnInteractWithItem.Broadcast();
		return true;
	}
	return false;
}
