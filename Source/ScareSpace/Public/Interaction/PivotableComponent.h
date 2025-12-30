// Copyright Carter Wooton

#pragma once

#include "CoreMinimal.h"
#include "Interaction/InteractableComponent.h"
#include "PivotableComponent.generated.h"

class UPhysicsConstraintComponent;

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

	// Does this object snap shut/open?
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Latching")
	bool bEnableLatching = true;

	// Angle tolerance (degrees) to trigger the latch (e.g., 0-10 deg)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Latching")
	float LatchAngleThreshold = 10.0f;

	// How strong the "Magnet" pulls
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Latching")
	float LatchStrength = 2000.0f;

	// Movement Limits (Used by Interactor for Clamping)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Limits")
	bool bIsLinearMovement = false; // False = Rotating Door, True = Sliding Drawer

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Limits")
	float MinLimit = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Limits")
	float MaxLimit = 90.0f;

protected:
	virtual void BeginPlay() override;

private:
	UFUNCTION()
	void OnMeshWake(UPrimitiveComponent* WakingComponent, FName BoneName);

	// Gets set so we don't have to keep looking for it
	UPROPERTY()
	TObjectPtr<UPrimitiveComponent> PivotableMesh = nullptr;

	// For dealing with the angular motor
	UPROPERTY()
	TObjectPtr<UPhysicsConstraintComponent> ConstraintComponent;

	bool bIsBeingHeld = false;
};
