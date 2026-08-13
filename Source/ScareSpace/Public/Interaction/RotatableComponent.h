// Copyright Carter Wooton

#pragma once

#include "CoreMinimal.h"
#include "Interaction/InteractableComponent.h"
#include "RotatableComponent.generated.h"

UENUM(BlueprintType)
enum class ERotationAxis : uint8
{
	Pitch, // Y Axis
	Yaw,   // Z Axis
	Roll   // X Axis
};

UCLASS(ClassGroup = (Interaction), meta = (BlueprintSpawnableComponent))
class SCARESPACE_API URotatableComponent : public UInteractableComponent
{
	GENERATED_BODY()

public:
	URotatableComponent();

	virtual bool BeginInteraction(UInteractorComponent* Interactor) override;
	virtual void EndInteraction(UInteractorComponent* Interactor) override;
	virtual void ProcessInputDelta(FVector2D InputDelta, UInteractorComponent* Interactor) override;
	virtual bool IsBoundToMesh(UPrimitiveComponent* HitMesh) const override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rotatable|Components")
	FName RotatableMeshName;

	UPROPERTY(BlueprintReadOnly, Transient, Category = "Rotatable|Components")
	TObjectPtr<USceneComponent> RotatableMeshComponent;

	// Which local axis the component should spin around
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rotatable|Settings")
	ERotationAxis RotationAxis = ERotationAxis::Roll;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rotatable|Settings")
	float RotationSensitivity = 3.0f;

	// If false, the crank can spin infinitely
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rotatable|Limits")
	bool bHasLimits = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rotatable|Limits", meta = (EditCondition = "bHasLimits"))
	float MinAngle = 0.0f;

	// Default is 1080 degrees (3 full rotations)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rotatable|Limits", meta = (EditCondition = "bHasLimits"))
	float MaxAngle = 1080.0f;

	// Expose current rotation for puzzle logic (e.g., checking if crank is fully turned)
	UFUNCTION(BlueprintPure, Category = "Rotatable|State")
	float GetCurrentAngle() const { return CurrentAngle; }

protected:
	virtual void BeginPlay() override;

private:
	bool bIsBeingHeld = false;

	// Tracked mathematically to prevent Euler angle normalization issues
	float CurrentAngle = 0.0f;
	FQuat BaseRotationQuat;
};