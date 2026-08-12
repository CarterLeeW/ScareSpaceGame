// Copyright Carter Wooton

#include "Interaction/ItemInteractableComponent.h"

UItemInteractableComponent::UItemInteractableComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UItemInteractableComponent::BeginPlay()
{
	Super::BeginPlay();

	TArray<UActorComponent*> AllComponents;
	GetOwner()->GetComponents(AllComponents);

	for (UActorComponent* Comp : AllComponents)
	{
		if (Comp->GetFName() == ItemOnlyParentComponentName)
		{
			ItemOnlyParentComponent = Comp;
			break;
		}
	}

	if (!ItemOnlyParentComponent)
	{
		ItemOnlyParentComponent = GetAttachParent();
	}
}

bool UItemInteractableComponent::IsBoundToMesh(UPrimitiveComponent* HitMesh) const
{
	if (!IsValid(HitMesh))
	{
		return false;
	}

	if (ItemOnlyParentComponent && HitMesh == ItemOnlyParentComponent)
	{
		return true;
	}

	return Super::IsBoundToMesh(HitMesh);
}