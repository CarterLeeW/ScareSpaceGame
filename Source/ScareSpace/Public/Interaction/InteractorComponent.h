// Copyright Carter Wooton

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "InteractorComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInteractionIconChanged, UTexture2D*, NewIcon);
// Can be for tutorials that need to track number of interactions had
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInteractionBeginsInteractor);

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

	// Holding object Context
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputMappingContext> HoldingObjectMappingContext;

	// Pivoting Context
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputMappingContext> PivotingMappingContext;

	// Holding item context
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputMappingContext> HoldingItemMappingContext;

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

	/* Drop held item Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	TObjectPtr<UInputAction> DropHeldItemAction;

	/* End input actions */

	// The controller that owns this interactor component
	TObjectPtr<APlayerController> ThisController;

	// The character that owns this interactor component
	TObjectPtr<ACharacter> ThisCharacter;

	// Event called when the interaction icon should be changed
	UPROPERTY(BlueprintAssignable, Category = "Interaction")
	FOnInteractionIconChanged OnInteractionIconChanged;

	// Event called when an interaction begins
	UPROPERTY(BlueprintAssignable, Category = "Interaction")
	FOnInteractionBeginsInteractor OnInteractionBegins;

	UPROPERTY(BlueprintReadOnly, Category = "Interaction")
	uint8 PivotableInteractionCounter = 0;
	UPROPERTY(BlueprintReadOnly, Category = "Interaction")
	uint8 CollectableInteractionCounter = 0;
	UPROPERTY(BlueprintReadOnly, Category = "Interaction")
	uint8 HoldableInteractionCounter = 0;
	UPROPERTY(BlueprintReadOnly, Category = "Interaction")
	uint8 ItemOnlyInteractionCounter = 0;
protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	/* For interaction input -Started- */
	void BeginInteraction();
	void StopHoldingItem();
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
	void BeginHoldingObject();
	// Called during ContinueInteraction to continue holding
	void ContinueHoldingObject();
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
