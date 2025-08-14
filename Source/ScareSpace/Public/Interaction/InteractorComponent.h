// Copyright Carter Wooton

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "InteractorComponent.generated.h"

class UInputMappingContext;
class UInputAction;
class UInteractableComponent;
class UPhysicsHandleComponent;

UCLASS( ClassGroup=(Interacting), meta=(BlueprintSpawnableComponent) )
class SCARESPACE_API UInteractorComponent : public USceneComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UInteractorComponent();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// Attributes

	/* Interaction */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction", meta = (ClampMin = "0.0"))
	float HandLength = 250.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction", meta = (ClampMin = "0.0"))
	float HoldAutoDropDistance = 300.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction", meta = (ClampMin = "0.0"))
	float MaxHoldLength = 200.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction", meta = (ClampMin = "0.0"))
	float MinHoldLength = 50.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction", meta = (ClampMin = "0.0"))
	float ThrowForceMultiplier = 10.0f;

	// Mapping Context
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputMappingContext> InteractorMappingContext;

	// Holding Context
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputMappingContext> HoldingMappingContext;

	/* Begin input actions */
	/* Interaction Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	TObjectPtr<UInputAction> InteractAction;

	/* Throw Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	TObjectPtr<UInputAction> ThrowAction;

	/* End input actions */

	// The controller that owns this interactor component
	TObjectPtr<APlayerController> ThisController;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	/* For interaction input -Started- */
	void BeginInteraction();
	/* For mouse release - requests the interaction to end -Completed- */
	void RequestEndInteraction();
	/* Logic for continuing an interaction (hold, turn, etc.) -Triggered- */
	void ContinueInteraction();
	/* For throwing objects that are being held */
	void ThrowObject();

private:
	/**
	* Performs a single trace along the visibility channel for objects that are within arms length of the character
	* @param OutHit
	*/
	void ArmsLengthTrace(FHitResult& OutResult);

	// Called when the interactable object is to be held
	void BeginHolding();
	// Called during ContinueInteraction to continue holding
	void ContinueHolding();
	// Current length object is being held at
	float TargetHoldLength = 0.0f;

	// The result of the ArmsLengthTrace(). Can be further evaluated
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	FHitResult ReachableTargetHitResult;

	// Used to determing if a mouse click/release may cause an interaction to occur
	bool bIsInteracting = false;

	// Component this controller is currently interracting with - will be nullptr if no interaction
	TObjectPtr<UInteractableComponent> CurrentInteractableComponent = nullptr;

	// This owners PhysicsHandle
	TObjectPtr<UPhysicsHandleComponent> PhysicsHandle;
};
