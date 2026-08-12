// Copyright Carter Wooton

#pragma once

#include "CoreMinimal.h"
#include "Interaction/InteractableComponent.h"
#include "ItemInteractableComponent.generated.h"

UCLASS(ClassGroup = (Interaction), meta = (BlueprintSpawnableComponent))
class SCARESPACE_API UItemInteractableComponent : public UInteractableComponent
{
	GENERATED_BODY()

public:
	UItemInteractableComponent();

	virtual bool IsBoundToMesh(UPrimitiveComponent* HitMesh) const override;

	// This is a fallback for if this component is not properly made a child of its corresponding component
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemInteractable|Components")
	FName ItemOnlyParentComponentName;

	UPROPERTY(BlueprintReadOnly, Transient, Category = "ItemInteractable|Components")
	TObjectPtr<UActorComponent> ItemOnlyParentComponent;

protected:
	virtual void BeginPlay() override;
};