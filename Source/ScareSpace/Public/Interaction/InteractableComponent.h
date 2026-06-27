// Copyright Carter Wooton

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InteractableComponent.generated.h"

// Not used anywhere currently
DECLARE_MULTICAST_DELEGATE(OnInteractionEnded);

/* Label for the type of interactable component */
UENUM(BlueprintType)
enum class EInteractableType : uint8
{
	Collectable,
	Holdable,
	Pivotable,
	Slidable,
	ItemOnly
};

UCLASS(Abstract, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class SCARESPACE_API UInteractableComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UInteractableComponent();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Interactable")
	EInteractableType InteractableType;

	// Called when interaction is set to begin
	UFUNCTION(BlueprintCallable)
	virtual void BeginInteraction();

	// Called when interaction is set to end
	UFUNCTION(BlueprintCallable)
	virtual void EndInteraction();

	UFUNCTION(BlueprintCallable)
	virtual bool QuickValidateItemInteraction(FDataTableRowHandle CollectableItemRow);

	// Called when an interaction is performed with an item
	UFUNCTION(BlueprintCallable, Category = "Interaction")
	virtual bool TryInteractWithItem(FDataTableRowHandle CollectableItemRow);

	// Not used anywhere currently
	OnInteractionEnded InteractionEnded;

	// HUD icon for this interactable
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
	TObjectPtr<UTexture2D> InteractionIcon;

protected:
	// Items that may cause an interaction to occur. If empty, no interaction will occur anyway
	UPROPERTY(EditAnywhere)
	FDataTableRowHandle InteractableItem;

};
