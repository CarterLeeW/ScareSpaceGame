// Copyright Carter Wooton

#pragma once

#include "CoreMinimal.h"
#include "Interaction/InteractableComponent.h"
#include "RotatableComponent.generated.h"

UENUM(BlueprintType)
enum class ERotationAxis : uint8
{
	Pitch,
	Yaw,
	Roll
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnRotatableComponentRotated, float, NewAngle);

UCLASS(ClassGroup = (Interaction), meta = (BlueprintSpawnableComponent))
class SCARESPACE_API URotatableComponent : public UInteractableComponent
{
	GENERATED_BODY()

public:
	URotatableComponent();

	virtual bool BeginInteraction(UInteractorComponent* Interactor) override;
	virtual void ContinueInteraction(UInteractorComponent* Interactor) override;
	virtual void EndInteraction(UInteractorComponent* Interactor) override;
	virtual void ProcessInputDelta(FVector2D InputDelta, UInteractorComponent* Interactor) override;
	virtual bool IsBoundToMesh(UPrimitiveComponent* HitMesh) const override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rotatable|Components")
	FName RotatableMeshName;

	UPROPERTY(BlueprintReadOnly, Transient, Category = "Rotatable|Components")
	TObjectPtr<USceneComponent> RotatableMeshComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rotatable|Settings")
	ERotationAxis RotationAxis = ERotationAxis::Roll;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rotatable|Settings")
	float RotationSensitivity = 3.0f;

	// Allows configuration of reversed controls per-mesh
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rotatable|Settings")
	bool bInvertRotation = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rotatable|Settings")
	bool bCanRotate = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rotatable|Limits")
	bool bHasLimits = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rotatable|Limits", meta = (EditCondition = "bHasLimits"))
	float MinAngle = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rotatable|Limits", meta = (EditCondition = "bHasLimits"))
	float MaxAngle = 1080.0f;

	UFUNCTION(BlueprintPure, Category = "Rotatable|State")
	float GetCurrentAngle() const { return CurrentAngle; }

	UPROPERTY(BlueprintAssignable, Category = "Rotatable|Events")
	FOnRotatableComponentRotated OnRotated;

protected:
	virtual void BeginPlay() override;

private:
	bool bIsBeingHeld = false;
	float CurrentAngle = 0.0f;
	FQuat BaseRotationQuat;

	FVector2D VirtualMousePosition = FVector2D::ZeroVector;
	float PreviousMouseAngle = 0.0f;
};