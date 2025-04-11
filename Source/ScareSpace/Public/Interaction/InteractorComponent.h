// Copyright Carter Wooton

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "InteractorComponent.generated.h"

class UInputMappingContext;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SCARESPACE_API UInteractorComponent : public USceneComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UInteractorComponent();

	// Mapping Context
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputMappingContext> InteractorMappingContext;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
