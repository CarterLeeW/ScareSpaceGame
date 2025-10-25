// Copyright Carter Wooton

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h" 
#include "ItemStructs.generated.h"

// insert enum for the type of item:
// e.g. consumable, useable
// maybe this could just be bools?

/**
 * 
 */
USTRUCT(BlueprintType)
struct FItemData : public FTableRowBase
{
	GENERATED_BODY()

	// 1. Unique ID for easy lookups in C++ (can be used to track the item's identity)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Identity")
	FText ItemDisplayName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Identity")
	FText ItemDescription;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Cosmetic")
	TSoftObjectPtr<UTexture2D> ItemIcon;
};
