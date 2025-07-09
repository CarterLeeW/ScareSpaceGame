// Copyright Carter Wooton


#include "Interaction/HoldableComponent.h"
#include "Components/StaticMeshComponent.h"

UHoldableComponent::UHoldableComponent()
{
	InteractableType = EInteractableType::Holdable;
}

void UHoldableComponent::BeginInteraction()
{
	UE_LOG(LogTemp, Display, TEXT("Holdable component begin implementation"));
	// Disable blocking collision with pawns so that the player can walk through the object and not stand on it etc.
	if (UStaticMeshComponent* Mesh = GetOwner()->GetComponentByClass<UStaticMeshComponent>())
	{
		Mesh->SetCollisionResponseToChannel(ECC_Pawn, ECollisionResponse::ECR_Overlap);
	}
}

void UHoldableComponent::EndInteraction()
{
	UE_LOG(LogTemp, Display, TEXT("Holdable component end implementation"));
	// Re-enable blocking collision with pawns
	if (UStaticMeshComponent* Mesh = GetOwner()->GetComponentByClass<UStaticMeshComponent>())
	{
		Mesh->SetCollisionResponseToChannel(ECC_Pawn, ECollisionResponse::ECR_Block);
	}
	// TODO: Delegate functionality is currently unnecessary
	/*InteractionEnded.Broadcast();
	InteractionEnded.Clear();*/
}
