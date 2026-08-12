// Copyright Carter Wooton

#pragma once

#include "CoreMinimal.h"
#include "Interaction/InteractableComponent.h"
#include "PivotableComponent.generated.h"

class UPhysicsConstraintComponent;
class UStaticMeshComponent;

UCLASS(ClassGroup = (Interaction), meta = (BlueprintSpawnableComponent))
class SCARESPACE_API UPivotableComponent : public UInteractableComponent
{
	GENERATED_BODY()

public:
	UPivotableComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	virtual bool BeginInteraction(UInteractorComponent* Interactor) override;
	virtual void ContinueInteraction(UInteractorComponent* Interactor) override;
	virtual void EndInteraction(UInteractorComponent* Interactor) override;
	virtual void ProcessInputDelta(FVector2D InputDelta, UInteractorComponent* Interactor) override;
	virtual bool TryInteractWithItem(const FDataTableRowHandle& CollectableItemRow, UInteractorComponent* Interactor) override;
	virtual bool IsBoundToMesh(UPrimitiveComponent* HitMesh) const override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pivotable|Components")
	FName PivotableParentMeshName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pivotable|Components")
	FName HingeComponentName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pivotable|Components")
	FName PhysicsConstraintName;

	UPROPERTY(BlueprintReadOnly, Transient, Category = "Pivotable|Components")
	TObjectPtr<UStaticMeshComponent> PivotableParentMeshComponent;

	UPROPERTY(BlueprintReadOnly, Transient, Category = "Pivotable|Components")
	TObjectPtr<USceneComponent> HingeComponent;

	UPROPERTY(BlueprintReadOnly, Transient, Category = "Pivotable|Components")
	TObjectPtr<UPhysicsConstraintComponent> PhysicsConstraintComponent;

	bool GetIsLocked() const { return bIsLocked; }
	void SetIsLocked(bool bNewIsLocked) { bIsLocked = bNewIsLocked; }

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pivotable")
	FRotator HingeStartingRotation = FRotator::ZeroRotator;
	FRotator BaseRotation;

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
	void OnUnlocked();

private:
	float TargetHoldLength = 0.0f;
	float TargetSideLength = 0.0f;
};