// Copyright Carter Wooton

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "InteractorComponent.generated.h"

class UInputMappingContext;
class UInputAction;
class UInteractableComponent;

UCLASS( ClassGroup=(Interacting), meta=(BlueprintSpawnableComponent) )
class SCARESPACE_API UInteractorComponent : public USceneComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UInteractorComponent();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// Mapping Context
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputMappingContext> InteractorMappingContext;

	/* Interaction Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	TObjectPtr<UInputAction> InteractAction;
	/* End input actions */

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	/* For interaction input */
	void BeginInteraction();
	/* For mouse release - requests the interaction to end */
	void RequestEndInteraction();

private:
	/**
	* Performs a single trace along the visibility channel for objects that are within arms length of the character
	* @param OutHit
	*/
	void ArmsLengthTrace(FHitResult& OutResult);

	// The result of the ArmsLengthTrace(). Can be further evaluated
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	FHitResult ReachableTargetHitResult;

	// Used to determing if a mouse click/release may cause an interaction to occur
	bool bIsInteracting = false;

	// Component this controller is currently interracting with - will be nullptr if no interaction
	TObjectPtr<UInteractableComponent> CurrentInteractableComponent = nullptr;
};
