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

	// Called when interaction is set to end
	virtual void EndInteraction() override;

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// Explicit references to the specific components driving this pivotable object
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pivotable|Components")
	TObjectPtr<UStaticMeshComponent> PivotableParentMeshComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pivotable|Components")
	TObjectPtr<USceneComponent> HingeComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pivotable|Components")
	TObjectPtr<UPhysicsConstraintComponent> PhysicsConstraintComponent;

	bool GetIsBeingHeld() const { return bIsBeingHeld; }
	void SetIsBeingHeld(bool bNewIsBeingHeld) { bIsBeingHeld = bNewIsBeingHeld; }
	bool GetIsLocked() const { return bIsLocked; }
	void SetIsLocked(bool bNewIsLocked) { bIsLocked = bNewIsLocked; }
	bool GetIsClosed() const { return bIsClosed; }
	bool GetCanClose() const { return bCanClose; }
	float GetClosedAngle() const { return ClosedAngle; }
	FRotator GetBaseRotation() const { return BaseRotation; }

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
};
