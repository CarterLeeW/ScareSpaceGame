// Copyright Carter Wooton

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h" 
#include "ItemStructs.generated.h"

/**
 * For Items that can be collected and used by the inventory system
 */
USTRUCT(BlueprintType)
struct FItemDataInventory : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Identity")
	FText ItemDisplayName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Identity")
	FText ItemDescription;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Cosmetic")
	TObjectPtr<UTexture2D> ItemIcon;
};

UENUM(BlueprintType)
enum class EJournalItemType : uint8
{
	Memento,
	Note
};

/**
 * For Notes or other collectables that are stored in the journal
 */
USTRUCT(BlueprintType)
struct FItemDataJournal : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Identity")
	FText ItemDisplayName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Identity")
	FText ItemDescription;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Identity")
	EJournalItemType JournalItemSubtype;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Cosmetic", meta = (MultiLine = true))
	FText ItemTextContent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Cosmetic")
	TObjectPtr<UTexture2D> ItemBackground;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Cosmetic")
	TObjectPtr<UTexture2D> ItemIcon;

};
