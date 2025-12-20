// Copyright Carter Wooton

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "InputMappingContext.h"
#include "InputConfigData.generated.h"

/**
 * 
 */
UCLASS()
class SCARESPACE_API UInputConfigData : public UDataAsset
{
	GENERATED_BODY()
	
public:
    // Every context that could be active during normal gameplay
    UPROPERTY(EditAnywhere, Category = "Gameplay")
    TArray<UInputMappingContext*> AllGameplayContexts;

    // The context(s) used only for the Inventory/Menu
    UPROPERTY(EditAnywhere, Category = "Menu")
    TArray<UInputMappingContext*> MenuContexts;
};
