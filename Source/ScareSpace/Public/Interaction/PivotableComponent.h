// Copyright Carter Wooton

#pragma once

#include "CoreMinimal.h"
#include "Interaction/InteractableComponent.h"
#include "PivotableComponent.generated.h"

/**
 * 
 */
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
	
	// This is the highest component in the hierarchy that moves when pivoting
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Pivotable")
	FName PivotableParentMeshName;

	// This is the hinge for the pivoting motion
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pivotable")
	FName HingeComponentName;

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

	// Should be set on the interactor when beginning to pivot
	UPROPERTY(BlueprintReadOnly, Category = "Pivotable")
	bool bIsBeingHeld = false;
	bool bIsLocked = true;
	bool bIsClosed = false;
	bool bCanClose = false;
	float ClosedAngle = 5.0f;
	FRotator BaseRotation;

	// The angle the pivotable starts at (like for an open door) Leave hinge at zero in editor
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pivotable")
	FRotator HingeStartingRotation = FRotator::ZeroRotator;

	TObjectPtr<UStaticMeshComponent> PivotableParentMeshComponent;
	TObjectPtr<USceneComponent> HingeComponent;

	void UpdateClosedState();
};
