// Copyright Carter Wooton

#include "Interaction/InteractableComponent.h"
#include "Interaction/InteractorComponent.h"

UInteractableComponent::UInteractableComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	PrimaryComponentTick.bStartWithTickEnabled = false;
}

bool UInteractableComponent::CanInteract(UInteractorComponent* Interactor) const
{
	return IsValid(Interactor);
}

bool UInteractableComponent::BeginInteraction(UInteractorComponent* Interactor)
{
	if (!CanInteract(Interactor))
	{
		return false;
	}
	if (!bCanInteract)
	{
		return false;
	}

	InteractionCounter = FMath::Clamp(InteractionCounter + 1, 0, 255);
	OnInteractionBegins.Broadcast(Interactor);
	return true;
}

void UInteractableComponent::ContinueInteraction(UInteractorComponent* Interactor)
{
}

void UInteractableComponent::EndInteraction(UInteractorComponent* Interactor)
{
	OnInteractionEnds.Broadcast(Interactor);
}

void UInteractableComponent::ProcessInputDelta(FVector2D InputDelta, UInteractorComponent* Interactor)
{
}

void UInteractableComponent::OnThrow(UInteractorComponent* Interactor)
{
}

bool UInteractableComponent::QuickValidateItemInteraction(const FDataTableRowHandle& CollectableItemRow) const
{
	if (!bCanAcceptItem || InteractableItem.IsNull())
	{
		return false;
	}
	return InteractableItem == CollectableItemRow;
}

bool UInteractableComponent::TryInteractWithItem(const FDataTableRowHandle& CollectableItemRow, UInteractorComponent* Interactor)
{
	if (!bCanAcceptItem)
	{
		return false;
	}
	if (!InteractableItem.IsNull() && CollectableItemRow == InteractableItem)
	{
		OnInteractWithItem.Broadcast(Interactor);
		return true;
	}
	return false;
}

bool UInteractableComponent::IsBoundToMesh(UPrimitiveComponent* HitMesh) const
{
	if (!IsValid(HitMesh))
	{
		return false;
	}

	// 1. Direct attachment check (preferred modular architecture)
	if (GetAttachParent() == HitMesh)
	{
		return true;
	}

	// 2. Hierarchy parent check
	TArray<USceneComponent*, FDefaultAllocator> Parents;
	HitMesh->GetParentComponents(Parents);
	for (USceneComponent* ParentComp : Parents)
	{
		if (ParentComp == GetAttachParent())
		{
			return true;
		}
	}

	return false;
}

UTexture2D* UInteractableComponent::GetInteractionIcon(UInteractorComponent* Interactor) const
{
	return InteractionIcon;
}