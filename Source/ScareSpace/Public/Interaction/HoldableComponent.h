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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
	bool bIsBreakable = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
	float BreakForce = 1000.0f;

protected:
	virtual void BeginPlay() override;

private:
	// Cannot break when being held
	bool bIsBeingHeld = false;
	
};
