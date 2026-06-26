// Copyright Carter Wooton

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "InteractorComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInteractionIconChanged, UTexture2D*, NewIcon);

class UInputMappingContext;
class UInputAction;
class UInteractableComponent;
class UPhysicsHandleComponent;
class UPivotableComponent;
class UInventoryComponent;
struct FInputActionValue;

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

	// Pivoting Context
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputMappingContext> PivotingMappingContext;

	/* Begin input actions */
	/* Interaction Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	TObjectPtr<UInputAction> InteractAction;

	/* Throw Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	TObjectPtr<UInputAction> ThrowAction;

	/* Push Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	TObjectPtr<UInputAction> PushAction;

	/* Pivot Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	TObjectPtr<UInputAction> PivotAction;

	/* End input actions */

	// The controller that owns this interactor component
	TObjectPtr<APlayerController> ThisController;

	// The character that owns this interactor component
	TObjectPtr<ACharacter> ThisCharacter;

	// Event called when the interaction icon should be changed
	UPROPERTY(BlueprintAssignable, Category = "Interaction")
	FOnInteractionIconChanged OnInteractionIconChanged;

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
	/* For pushing objects that are being pivoted */
	void PushObject();

	UFUNCTION()
	void HandleOnItemSelected(FDataTableRowHandle SelectedItemRow);

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

	// Called when the interactable object is to be pivoted
	void BeginPivoting();
	// Called during ContinueInteraction to continue pivoting
	void ContinuePivoting();
	// Replaces look actions to calculate pivot parameters
	void CalculateLateralOffset(const FInputActionValue& Value);
	// Left/Right offset from center when pivoting
	float TargetSideLength = 0.0f;

	// Called when interacting with a collectable item
	void Collect();

	// The result of the ArmsLengthTrace(). Can be further evaluated
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	FHitResult ReachableTargetHitResult;

	// Used to determing if a mouse click/release may cause an interaction to occur
	bool bIsInteracting = false;

	// Component this controller is currently interracting with - will be nullptr if no interaction
	TObjectPtr<UInteractableComponent> CurrentInteractableComponent = nullptr;

	// This owners PhysicsHandle
	TObjectPtr<UPhysicsHandleComponent> PhysicsHandle;

	// True if the touched component is part of a moveable unit (ex. handle) - false if it is static (ex. door frame)
	bool IsChildOfPivotableComponent(UPrimitiveComponent* TargetedComponent, UPivotableComponent* PivotableComp);

	// Update the interaction prompt icon and other stuff if applicable
	void UpdateInteractionPrompt();

	// Track the last icon to prevent spamming updates
	TWeakObjectPtr<UTexture2D> LastInteractionIcon = nullptr;

	TObjectPtr<UInventoryComponent> InventoryComponent;
	// Lightweight tracker for what row identifier is active
	FDataTableRowHandle ActiveHeldItemRow;

	TObjectPtr<UTexture2D> CachedActiveItemIcon = nullptr;
	bool bHoldingItem = false;
};
