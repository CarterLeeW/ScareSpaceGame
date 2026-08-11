// Copyright Carter Wooton


#include "Interaction/ItemInteractableComponent.h"

UItemInteractableComponent::UItemInteractableComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	PrimaryComponentTick.bStartWithTickEnabled = false;
	InteractableType = EInteractableType::ItemOnly;
}

void UItemInteractableComponent::BeginPlay()
{
	Super::BeginPlay();

	// Retrieve all components attached to the owning actor
	TArray<UActorComponent*> AllComponents;
	GetOwner()->GetComponents(AllComponents);

	// Iterate through and match the FNames
	for (UActorComponent* Comp : AllComponents)
	{
		if (Comp->GetFName() == ItemOnlyParentMeshName)
		{
			ItemOnlyParentComponent = Comp;
		}
	}
}
