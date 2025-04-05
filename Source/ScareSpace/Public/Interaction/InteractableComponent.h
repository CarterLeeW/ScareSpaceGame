// Copyright Carter Wooton

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "InteractableComponent.generated.h"

/* Label for the type of interactable component */
UENUM(BlueprintType)
enum class EInteractableType : uint8
{
	Collectable,
	Holdable,
	Door,
	Pivotable
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

};
