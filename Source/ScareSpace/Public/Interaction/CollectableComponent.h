// Copyright Carter Wooton

#pragma once

#include "CoreMinimal.h"
#include "Interaction/InteractableComponent.h"
#include "CollectableComponent.generated.h"

/**
 * 
 */
UCLASS(ClassGroup = (Interactable), meta = (BlueprintSpawnableComponent))
class SCARESPACE_API UCollectableComponent : public UInteractableComponent
{
	GENERATED_BODY()

public:
	UCollectableComponent();
	
};
