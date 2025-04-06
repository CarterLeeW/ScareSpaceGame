// Copyright Carter Wooton


#include "Interaction/InteractableComponent.h"

UInteractableComponent::UInteractableComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UInteractableComponent::BeginInteraction()
{
	UE_LOG(LogTemp, Display, TEXT("Begin interaction Base Implementation"));
}

void UInteractableComponent::EndInteraction()
{
	UE_LOG(LogTemp, Display, TEXT("End interaction Base Implementation"));
}
