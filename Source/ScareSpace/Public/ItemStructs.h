// Copyright Carter Wooton

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h" 
#include "ItemStructs.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct FItemData : public FTableRowBase
{
	GENERATED_BODY()

	// 1. Unique ID for easy lookups in C++ (can be used to track the item's identity)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Identity")
	int32 ItemID;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Identity")
	FText ItemName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Identity")
	FText ItemDescription;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Properties")
	bool bIsStackable;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Properties")
	int32 MaxStackSize;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Cosmetic")
	TSoftObjectPtr<UTexture2D> ItemIcon;
};
