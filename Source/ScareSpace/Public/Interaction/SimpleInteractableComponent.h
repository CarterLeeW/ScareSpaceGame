// Copyright Carter Wooton

#pragma once

#include "CoreMinimal.h"
#include "Interaction/InteractableComponent.h"
#include "SimpleInteractableComponent.generated.h"

// Delegate for wiring up Blueprint logic
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSimpleInteract);

UCLASS(ClassGroup = (Interaction), meta = (BlueprintSpawnableComponent))
class SCARESPACE_API USimpleInteractableComponent : public UInteractableComponent
{
	GENERATED_BODY()

public:
	// Broadcasts to the Blueprint Event Graph when clicked
	UPROPERTY(BlueprintAssignable, Category = "Interaction|Events")
	FOnSimpleInteract OnInteracted;

	virtual bool BeginInteraction(UInteractorComponent* Interactor) override;
};