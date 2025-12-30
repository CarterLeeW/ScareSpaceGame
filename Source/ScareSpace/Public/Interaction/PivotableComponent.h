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

protected:
	virtual void BeginPlay() override;

	UPROPERTY(BlueprintReadOnly, Category = "Pivotable")
	bool bIsBeingHeld = false;
};
