// Copyright Carter Wooton

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "Engine/DataTable.h"
#include "InteractorComponent.generated.h"

class UInputMappingContext;
class UInputAction;
class UInteractableComponent;
class UPhysicsHandleComponent;
class UInventoryComponent;
class UJournalComponent;
class APlayerController;
class ACharacter;
struct FInputActionValue;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnInteractionIconChanged, UTexture2D*, NewIcon, bool, bIsValidTarget);

UCLASS(ClassGroup = (Interaction), meta = (BlueprintSpawnableComponent))
class SCARESPACE_API UInteractorComponent : public USceneComponent
{
	GENERATED_BODY()

public:
	UInteractorComponent();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// Configuration Parameters
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction", meta = (ClampMin = "0.0"))
	float HandLength = 250.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction", meta = (ClampMin = "0.0"))
	float HoldAutoDropDistance = 300.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction", meta = (ClampMin = "0.0"))
	float ThrowForceMultiplier = 10.0f;

	// Mapping Contexts
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputMappingContext> InteractorMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputMappingContext> HoldingObjectMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputMappingContext> PivotingMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputMappingContext> HoldingItemMappingContext;

	// Input Actions
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> InteractAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> ThrowAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> PushAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> PivotAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> DropHeldItemAction;

	UPROPERTY(BlueprintAssignable, Category = "Interaction")
	FOnInteractionIconChanged OnInteractionIconChanged;

	// Accessors for interactable components
	APlayerController* GetPlayerController() const { return ThisController; }
	ACharacter* GetCharacterOwner() const { return ThisCharacter; }
	UInventoryComponent* GetInventoryComponent() const { return InventoryComponent; }
	UJournalComponent* GetJournalComponent() const { return JournalComponent; }
	UPhysicsHandleComponent* GetPhysicsHandle() const { return PhysicsHandle; }
	const FHitResult& GetReachableTargetHitResult() const { return ReachableTargetHitResult; }
	const FDataTableRowHandle& GetActiveHeldItemRow() const { return ActiveHeldItemRow; }
	bool IsHoldingInventoryItem() const { return bHoldingItem; }

	void StopHoldingItem();
	void RequestEndInteraction();

protected:
	virtual void BeginPlay() override;

	void BeginInteraction();
	void ContinueInteraction();
	void ThrowObject();
	void ProcessLateralOffset(const FInputActionValue& Value);

	UFUNCTION()
	void HandleOnItemSelected(FDataTableRowHandle SelectedItemRow);

private:
	void ArmsLengthTrace();
	void UpdateInteractionPrompt();
	UInteractableComponent* FindInteractableForMesh(UPrimitiveComponent* HitMesh) const;

	FHitResult ReachableTargetHitResult;
	bool bIsInteracting = false;
	bool bHoldingItem = false;

	TWeakObjectPtr<UInteractableComponent> CurrentInteractableComponent;
	TWeakObjectPtr<UInteractableComponent> HoveredInteractableComponent;

	TObjectPtr<APlayerController> ThisController;
	TObjectPtr<ACharacter> ThisCharacter;
	TObjectPtr<UInventoryComponent> InventoryComponent;
	TObjectPtr<UJournalComponent> JournalComponent;
	TObjectPtr<UPhysicsHandleComponent> PhysicsHandle;

	FDataTableRowHandle ActiveHeldItemRow;
	TObjectPtr<UTexture2D> CachedActiveItemIcon;
	TWeakObjectPtr<UTexture2D> LastInteractionIcon;

	bool bLastTargetValid = false;
};