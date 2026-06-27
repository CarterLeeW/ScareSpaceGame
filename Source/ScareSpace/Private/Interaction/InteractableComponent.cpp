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
	if (InteractableItems.Num() == 0)
	{
		return false;
	}
	for (const FDataTableRowHandle& ItemHandle : InteractableItems)
	{
		if (ItemHandle == ItemRow)
		{
			return true;
		}
	}
	return false;
}

void UInteractableComponent::ItemInteraction(FDataTableRowHandle ItemRow)
{
	UE_LOG(LogInteraction, Display, TEXT("ItemInteraction called with item: %s"), *ItemRow.RowName.ToString());
}
