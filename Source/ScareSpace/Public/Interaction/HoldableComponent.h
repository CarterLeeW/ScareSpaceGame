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
	
};
