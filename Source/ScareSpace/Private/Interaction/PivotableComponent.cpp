// Copyright Carter Wooton


#include "Interaction/PivotableComponent.h"
#include "Logging/ScareSpaceLogs.h"

UPivotableComponent::UPivotableComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
    PrimaryComponentTick.bStartWithTickEnabled = true;
	InteractableType = EInteractableType::Pivotable;
}

void UPivotableComponent::BeginInteraction()
{
    if (bIsLocked)
    {
        // locked cosmetics
    }
    else
    {
        bIsBeingHeld = true;
    }
}

void UPivotableComponent::EndInteraction()
{
    bIsBeingHeld = false;
}

void UPivotableComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
}

void UPivotableComponent::BeginPlay()
{
	Super::BeginPlay();
	// Find the pivotable parent mesh component somewhere on this actor by name
    PivotableParentMeshComponent = Cast<UStaticMeshComponent>(GetOwner()->GetDefaultSubobjectByName(PivotableParentMeshName));
    if (!PivotableParentMeshComponent)
    {
        UE_LOG(LogInteraction, Warning, TEXT("PivotableComponent: Could not find PivotableParentMeshComponent with name %s on actor %s"), *PivotableParentMeshName.ToString(), *GetOwner()->GetName());
	}
    // do same for hinge component
	HingeComponent = Cast<USceneComponent>(GetOwner()->GetDefaultSubobjectByName(HingeComponentName));
    if (!HingeComponent)
    {
        UE_LOG(LogInteraction, Warning, TEXT("PivotableComponent: Could not find HingeComponent with name %s on actor %s"), *HingeComponentName.ToString(), *GetOwner()->GetName());
	}

	// Perform necessary default setup
	BaseRotation = PivotableParentMeshComponent->GetComponentRotation();
	HingeComponent->SetRelativeRotation(HingeStartingRotation);
	PivotableParentMeshComponent->SetSimulatePhysics(true);
}

void UPivotableComponent::UpdateClosedState()
{

}
