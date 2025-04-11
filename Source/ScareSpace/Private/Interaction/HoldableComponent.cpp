// Copyright Carter Wooton


#include "Interaction/HoldableComponent.h"

UHoldableComponent::UHoldableComponent()
{
	InteractableType = EInteractableType::Holdable;
}

void UHoldableComponent::BeginInteraction()
{
	UE_LOG(LogTemp, Display, TEXT("Holdable component begin implementation"));
	
}

void UHoldableComponent::EndInteraction()
{
	UE_LOG(LogTemp, Display, TEXT("Holdable component end implementation"));
	// TODO: Delegate functionality is currently unnecessary
	/*InteractionEnded.Broadcast();
	InteractionEnded.Clear();*/
}
