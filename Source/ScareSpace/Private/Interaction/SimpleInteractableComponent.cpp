// Copyright Carter Wooton


#include "Interaction/SimpleInteractableComponent.h"
#include "Interaction/InteractorComponent.h"

bool USimpleInteractableComponent::BeginInteraction(UInteractorComponent* Interactor)
{
	if (!IsValid(Interactor))
	{
		return false;
	}

	Super::BeginInteraction(Interactor);
	OnInteracted.Broadcast(Interactor);

	// Interactory should not transition into interacting state
	return false;
}
