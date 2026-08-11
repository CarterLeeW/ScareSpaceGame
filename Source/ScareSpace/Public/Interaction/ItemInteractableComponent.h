// Copyright Carter Wooton

#pragma once

#include "CoreMinimal.h"
#include "Interaction/InteractableComponent.h"
#include "ItemInteractableComponent.generated.h"

/**
 * 
 */
UCLASS(ClassGroup = (Interactable), meta = (BlueprintSpawnableComponent))
class SCARESPACE_API UItemInteractableComponent : public UInteractableComponent
{
	GENERATED_BODY()

public:
	UItemInteractableComponent();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemInteractable|Components")
	FName ItemOnlyParentMeshName;

	// Resolved pointers accessed by the Interactor Component at runtime
	UPROPERTY(BlueprintReadOnly, Transient, Category = "ItemInteractable|Components")
	TObjectPtr<UActorComponent> ItemOnlyParentComponent;
	
protected:
	virtual void BeginPlay() override;
};
