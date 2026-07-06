// Copyright Carter Wooton

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "JournalComponent.generated.h"

class UInputMappingContext;
class UInputAction;
class UUserWidget;
class AMainPlayerController;

// Define the event signature. It will send the complete set of item names.
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnJournalUpdatedSignature, const TArray<FDataTableRowHandle>&, NewJournalItems);


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SCARESPACE_API UJournalComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UJournalComponent();

	/* Input */
	// Journal Mapping Context during gameplay
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputMappingContext> JournalGameplayContext;

	// Journal Mapping Context during menus
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputMappingContext> JournalMenuContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> ToggleJournalMenuAction;

	// Toggle journal: will replace open/close actions
	void ToggleJournalMenu();

	UFUNCTION(BlueprintCallable, Category = "Journal")
	bool AddItemToJournal(FDataTableRowHandle CollectableItemRow);

	UFUNCTION(BlueprintCallable, Category = "Journal")
	bool RemoveItemFromJournal(FDataTableRowHandle CollectableItemRow);

	UPROPERTY(BlueprintAssignable, Category = "Journal|Events")
	FOnJournalUpdatedSignature OnJournalUpdated;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Journal")
	TArray<FDataTableRowHandle> JournalItems;

	//This is the main database of all journal items.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Journal")
	TObjectPtr<UDataTable> JournalItemDatabase;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UUserWidget> JournalWidgetClass;

	UPROPERTY()
	TObjectPtr<UUserWidget> JournalWidgetInstance;

	UPROPERTY(BlueprintReadOnly, Category = "Journal")
	bool bIsJournalOpen = false;

	// The controller that owns this component
	TObjectPtr<AMainPlayerController> ThisController;
};
