// Copyright Carter Wooton

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "Engine/DataTable.h"
#include "InteractableComponent.generated.h"

class UInteractorComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInteractionBegins, UInteractorComponent*, Interactor);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInteractionEnds, UInteractorComponent*, Interactor);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInteractWithItem, UInteractorComponent*, Interactor);

UCLASS(Abstract, Blueprintable, BlueprintType, ClassGroup = (Interaction), meta = (BlueprintSpawnableComponent))
class SCARESPACE_API UInteractableComponent : public USceneComponent
{
	GENERATED_BODY()

public:
	UInteractableComponent();

	// Polymorphic interaction hooks
	virtual bool CanInteract(UInteractorComponent* Interactor) const;
	virtual bool BeginInteraction(UInteractorComponent* Interactor);
	virtual void ContinueInteraction(UInteractorComponent* Interactor);
	virtual void EndInteraction(UInteractorComponent* Interactor);
	virtual void ProcessInputDelta(FVector2D InputDelta, UInteractorComponent* Interactor);
	virtual void OnThrow(UInteractorComponent* Interactor);

	// Item handling
	virtual bool QuickValidateItemInteraction(const FDataTableRowHandle& CollectableItemRow) const;
	virtual bool TryInteractWithItem(const FDataTableRowHandle& CollectableItemRow, UInteractorComponent* Interactor);

	// Target resolution
	virtual bool IsBoundToMesh(UPrimitiveComponent* HitMesh) const;

	// UI & Audio
	virtual UTexture2D* GetInteractionIcon(UInteractorComponent* Interactor) const;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
	bool bCanInteract = true;

	UPROPERTY(BlueprintAssignable, Category = "Interaction")
	FOnInteractWithItem OnInteractWithItem;

	UPROPERTY(BlueprintAssignable, Category = "Interaction")
	FOnInteractionBegins OnInteractionBegins;

	UPROPERTY(BlueprintAssignable, Category = "Interaction")
	FOnInteractionEnds OnInteractionEnds;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interaction|UI")
	TObjectPtr<UTexture2D> InteractionIcon;

	UPROPERTY(BlueprintReadOnly, Category = "Interaction")
	uint8 InteractionCounter = 0;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interaction|Lock/Item")
	FDataTableRowHandle InteractableItem;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction|Lock/Item")
	bool bCanAcceptItem = true;
};