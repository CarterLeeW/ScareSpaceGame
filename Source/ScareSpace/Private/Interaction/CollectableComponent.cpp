// Copyright Carter Wooton


#include "Interaction/CollectableComponent.h"
#include "Logging/ScareSpaceLogs.h"

UCollectableComponent::UCollectableComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	PrimaryComponentTick.bStartWithTickEnabled = false;
	InteractableType = EInteractableType::Collectable;
}

void UCollectableComponent::BeginInteraction()
{
	InteractionCounter = FMath::Clamp(InteractionCounter + 1, 0, 255);
	OnInteractionBegins.Broadcast();
}
