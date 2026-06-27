// Copyright Carter Wooton

#pragma once

#include "CoreMinimal.h"
#include "Interaction/InteractableComponent.h"
#include "PivotableComponent.generated.h"

class UPhysicsConstraintComponent;
class UStaticMeshComponent;
class USceneComponent;

/**
 * */
UCLASS(ClassGroup = (Interactable), meta = (BlueprintSpawnableComponent))
class SCARESPACE_API UPivotableComponent : public UInteractableComponent
{
	GENERATED_BODY()

public:
	UPivotableComponent();

	// Called when interaction is set to begin
	virtual void BeginInteraction() override;

	// Enter the exact name of the components as they appear in the Blueprint hierarchy
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pivotable|Components")
	FName PivotableParentMeshName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pivotable|Components")
	FName HingeComponentName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pivotable|Components")
	FName PhysicsConstraintName;
	// Resolved pointers accessed by the Interactor Component at runtime
	UPROPERTY(BlueprintReadOnly, Transient, Category = "Pivotable|Components")
	TObjectPtr<UStaticMeshComponent> PivotableParentMeshComponent;
	UPROPERTY(BlueprintReadOnly, Transient, Category = "Pivotable|Components")
	TObjectPtr<USceneComponent> HingeComponent;
	UPROPERTY(BlueprintReadOnly, Transient, Category = "Pivotable|Components")
	TObjectPtr<UPhysicsConstraintComponent> PhysicsConstraintComponent;

	// Called when interaction is set to end
	virtual void EndInteraction() override;

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	bool GetIsBeingHeld() const { return bIsBeingHeld; }
	void SetIsBeingHeld(bool bNewIsBeingHeld) { bIsBeingHeld = bNewIsBeingHeld; }
	bool GetIsLocked() const { return bIsLocked; }
	void SetIsLocked(bool bNewIsLocked) { bIsLocked = bNewIsLocked; }
	bool GetIsClosed() const { return bIsClosed; }
	bool GetCanClose() const { return bCanClose; }
	float GetClosedAngle() const { return ClosedAngle; }
	FRotator GetBaseRotation() const { return BaseRotation; }

	virtual bool TryInteractWithItem(FDataTableRowHandle ItemRow) override;
protected:
	virtual void BeginPlay() override;

	// The angle the pivotable starts at (like for an open door) Leave hinge at zero in editor
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pivotable")
	FRotator HingeStartingRotation = FRotator::ZeroRotator;
	FRotator BaseRotation;

	// Set on the interactor when beginning to pivot
	UPROPERTY(BlueprintReadOnly, Category = "Pivotable")
	bool bIsBeingHeld = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pivotable")
	bool bCanClose = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pivotable|Closing Details", meta = (EditCondition = "bCanClose"))
	float ClosedStrength = 1000.000f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pivotable|Closing Details", meta = (EditCondition = "bCanClose"))
	bool bIsLocked = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pivotable|Closing Details", meta = (EditCondition = "bCanClose"))
	bool bIsClosed = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pivotable|Closing Details", meta = (EditCondition = "bCanClose"))
	float ClosedAngle = 5.0f;

	void UpdateClosedState();
	// Cosmetics that fire when the pivotable is unlocked, like playing a sound or changing a material
	void OnUnlocked();
};
