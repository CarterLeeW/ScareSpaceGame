// Copyright Carter Wooton


#include "Interaction/CollectableComponent.h"
#include "Logging/ScareSpaceLogs.h"

UCollectableComponent::UCollectableComponent()
{
	InteractableType = EInteractableType::Collectable;
}

void UCollectableComponent::BeginInteraction()
{
	// Just test code
	UE_LOG(LogInteraction, Display, TEXT("Collectable interacted with, needs to interact with inventory"));
	// For a collectable, we just destroy the owner actor
	AActor* OwnerActor = GetOwner();
	if (IsValid(OwnerActor))
	{
		OwnerActor->Destroy();
	}
}
