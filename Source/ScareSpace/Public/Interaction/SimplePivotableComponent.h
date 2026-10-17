// Copyright Carter Wooton

#pragma once

#include "CoreMinimal.h"
#include "Interaction/InteractableComponent.h"
#include "SimplePivotableComponent.generated.h"

class UInputMappingContext;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSimplePivotableRestStateChanged, bool, bIsAtRest);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSimplePivotableLockStateChanged, bool, bIsLocked);

UCLASS(ClassGroup = (Custom), ClassGroup = (Interaction), meta = (BlueprintSpawnableComponent))
class SCARESPACE_API USimplePivotableComponent : public UInteractableComponent
{
	GENERATED_BODY()

public:
	USimplePivotableComponent();

	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	virtual bool BeginInteraction(UInteractorComponent* Interactor) override;
	virtual void EndInteraction(UInteractorComponent* Interactor) override;
	virtual bool TryInteractWithItem(const FDataTableRowHandle& CollectableItemRow, UInteractorComponent* Interactor) override;
	virtual void ProcessInputDelta(FVector2D InputDelta, UInteractorComponent* Interactor) override;

	UFUNCTION(BlueprintCallable, Category = "Interaction|Pivotable")
	void OpenPivotableProgrammatically(float TargetAngle = 90.0f, float Speed = 5.0f, bool bBypassLock = false);

	UFUNCTION(BlueprintCallable, Category = "Interaction|Pivotable")
	void ClosePivotableProgrammatically(float Speed = 5.0f);

	UPROPERTY(BlueprintAssignable, Category = "Interaction|Pivotable")
	FOnSimplePivotableRestStateChanged OnSimplePivotableRestStateChanged;

	UPROPERTY(BlueprintReadOnly, Category = "Interaction|Pivotable")
	bool bIsAtRest = true;
	
	/* Locking */
	UPROPERTY(BlueprintAssignable, Category = "Interaction|Pivotable")
	FOnSimplePivotableLockStateChanged OnSimplePivotableLockStateChanged;

	UFUNCTION(BlueprintCallable, Category = "Interaction|Pivot")
	void SetIsLocked(bool bInLocked);

	UPROPERTY(BlueprintReadOnly, Category = "Interaction|Pivotable")
	bool bIsGrabbed = false;

protected:
	UPROPERTY(EditAnywhere, Category = "Setup")
	FName PivotableParentMeshName = "PivotableMesh";

	UPROPERTY(EditAnywhere, Category = "Setup")
	float MaxOpenAngle = 90.0f;

	// Affects which way the door swings
	UPROPERTY(EditAnywhere, Category = "Setup")
	bool bReverseDirection = false;

	UPROPERTY(EditAnywhere, Category = "Setup")
	float ClosedAngleThreshold = 3.0f;

	UPROPERTY(EditAnywhere, Category = "Setup|Sensitivity")
	float DragSensitivity = 1.5f;

	// Affects which way the mouse wants to drag the door. 
	UPROPERTY(EditAnywhere, Category = "Setup|Sensitivity")
	bool bInvertDragAxis = false;

	UPROPERTY(EditAnywhere, Category = "Setup|Input")
	TObjectPtr<UInputMappingContext> PivotingMappingContext = nullptr;

	UPROPERTY(EditAnywhere, Category = "Setup|Input")
	int32 MappingContextPriority = 1;

	/* Locking */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Setup|Lock")
	bool bIsLocked = false;

	// Event for cosmetics
	void OnUnlocked();

private:
	UPROPERTY()
	TObjectPtr<UStaticMeshComponent> PivotableParentMeshComponent = nullptr;

	FRotator BaseRelativeRotation;
	float CurrentPivotAngle = 0.0f;
	float TargetPivotAngle = 0.0f;
	bool bIsInteractingFromBehind = false;

	bool bIsAnimatingProgrammatically = false;
	float ProgrammaticSpeed = 5.0f;

	void ApplyRotation(float NewAngle);
	void UpdateClosedState();

	void AddPivotingMappingContext(UInteractorComponent* Interactor);
	void RemovePivotingMappingContext(UInteractorComponent* Interactor);
};