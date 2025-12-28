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
	
	// This is the highest component in the hierarchy that moves when pivoting
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Pivotable")
	FName PivotableParentMeshName;
};
