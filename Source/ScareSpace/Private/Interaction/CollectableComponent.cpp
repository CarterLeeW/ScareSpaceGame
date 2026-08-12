// Copyright Carter Wooton

#include "Interaction/CollectableComponent.h"
#include "Interaction/InteractorComponent.h"
#include "Inventory/InventoryComponent.h"
#include "Journal/JournalComponent.h"
#include "Logging/ScareSpaceLogs.h"

UCollectableComponent::UCollectableComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

bool UCollectableComponent::BeginInteraction(UInteractorComponent* Interactor)
{
	if (!Super::BeginInteraction(Interactor))
	{
		return false;
	}

	bool bSuccessfullyCollected = false;

	if (CollectableType == ECollectableType::Inventory)
	{
		if (UInventoryComponent* InventoryComp = Interactor->GetInventoryComponent())
		{
			bSuccessfullyCollected = InventoryComp->AddItemToInventory(CollectableItemRow);
		}
	}
	else if (CollectableType == ECollectableType::Journal)
	{
		if (UJournalComponent* JournalComp = Interactor->GetJournalComponent())
		{
			bSuccessfullyCollected = JournalComp->AddItemToJournal(CollectableItemRow);
		}
	}

	if (bSuccessfullyCollected)
	{
		GetOwner()->Destroy();
		return true;
	}

	return false;
}