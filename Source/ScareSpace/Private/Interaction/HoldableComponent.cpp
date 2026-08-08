// Copyright Carter Wooton


#include "Interaction/HoldableComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Logging/ScareSpaceLogs.h"

UHoldableComponent::UHoldableComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	PrimaryComponentTick.bStartWithTickEnabled = false;
	InteractableType = EInteractableType::Holdable;
}

void UHoldableComponent::BeginInteraction()
{
	InteractionCounter = FMath::Clamp(InteractionCounter + 1, 0, 255);
	OnInteractionBegins.Broadcast();
	bIsBeingHeld = true;
	// Disable blocking collision with pawns so that the player can walk through the object and not stand on it etc.
	if (UStaticMeshComponent* Mesh = GetOwner()->GetComponentByClass<UStaticMeshComponent>())
	{
		Mesh->SetCollisionResponseToChannel(ECC_Pawn, ECollisionResponse::ECR_Overlap);
	}
}

void UHoldableComponent::EndInteraction()
{
	bIsBeingHeld = false;
	if (AActor* Owner = GetOwner())
	{
		// not reliable, does not show the items velocity after the impulse is applied
		FVector Velocity = Owner->GetVelocity();
		UE_LOG(LogInteraction, Display, TEXT("object velocity: %s"), *Velocity.ToString());
	}
	
	// Re-enable blocking collision with pawns
	if (UStaticMeshComponent* Mesh = GetOwner()->GetComponentByClass<UStaticMeshComponent>())
	{
		Mesh->SetCollisionResponseToChannel(ECC_Pawn, ECollisionResponse::ECR_Block);
	}
	// TODO: Delegate functionality is currently unnecessary
	/*InteractionEnded.Broadcast();
	InteractionEnded.Clear();*/
}

void UHoldableComponent::BeginPlay()
{
	Super::BeginPlay();

	// Bind OnComponentHit
	if (UStaticMeshComponent* Mesh = GetOwner()->GetComponentByClass<UStaticMeshComponent>())
	{
		Mesh->OnComponentHit.AddDynamic(this, &UHoldableComponent::OnMeshComponentHit);
	}
}

void UHoldableComponent::OnMeshComponentHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	// Example: Log the hit event
	//UE_LOG(LogInteraction, Display, TEXT("HoldableComponent was hit by %s"), *GetNameSafe(OtherActor));
	if (bIsBreakable && (NormalImpulse.Length() > BreakForce))
	{
		// breaks
		// TODO: Breakable needs to be its own component using chaos physics

	}
}
