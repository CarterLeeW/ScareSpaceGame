// Copyright Carter Wooton

#pragma once

#include "CoreMinimal.h"
#include "Interaction/InteractableComponent.h"
#include "CollectableComponent.generated.h"

UENUM(BlueprintType)
enum class ECollectableType : uint8
{
	Inventory,
	Journal
};

UCLASS(ClassGroup = (Interaction), meta = (BlueprintSpawnableComponent))
class SCARESPACE_API UCollectableComponent : public UInteractableComponent
{
	GENERATED_BODY()

public:
	UCollectableComponent();

	virtual bool BeginInteraction(UInteractorComponent* Interactor) override;

	UFUNCTION(BlueprintPure)
	FDataTableRowHandle GetItemRow() const { return CollectableItemRow; }

	UFUNCTION(BlueprintPure)
	ECollectableType GetCollectableType() const { return CollectableType; }

private:
	UPROPERTY(EditAnywhere, Category = "Collectable")
	FDataTableRowHandle CollectableItemRow;

	UPROPERTY(EditDefaultsOnly, Category = "Collectable")
	ECollectableType CollectableType = ECollectableType::Inventory;
};