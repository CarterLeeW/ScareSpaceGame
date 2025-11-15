// Copyright Carter Wooton


#include "Interaction/CollectableComponent.h"
#include "Logging/ScareSpaceLogs.h"

UCollectableComponent::UCollectableComponent()
{
	InteractableType = EInteractableType::Collectable;
}

void UCollectableComponent::BeginInteraction()
{
	UE_LOG(LogInteraction, Display, TEXT("This is where visual and sound effects are played by the collected item"));
}
