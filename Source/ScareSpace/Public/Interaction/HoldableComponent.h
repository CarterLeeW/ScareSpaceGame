// Copyright Carter Wooton

#pragma once

#include "CoreMinimal.h"
#include "Interaction/InteractableComponent.h"
#include "HoldableComponent.generated.h"

/**
 * 
 */
UCLASS(ClassGroup = (Interactable), meta = (BlueprintSpawnableComponent))
class SCARESPACE_API UHoldableComponent : public UInteractableComponent
{
	GENERATED_BODY()

public:
	UHoldableComponent();

	// Called when interaction is set to begin
	virtual void BeginInteraction() override;

	// Called when interaction is set to end
	virtual void EndInteraction() override;
	
};
