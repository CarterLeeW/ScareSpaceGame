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
    bIsBeingHeld = true;
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
    
}
