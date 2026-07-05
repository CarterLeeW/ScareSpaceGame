// Copyright Carter Wooton

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "JournalComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SCARESPACE_API UJournalComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UJournalComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable, Category = "Journal")
	void OpenJournalMenu();

	UFUNCTION(BlueprintCallable, Category = "Journal")
	void CloseJournalMenu();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Journal")
	TArray<FDataTableRowHandle> JournalItems;

	//This is the main database of all journal items.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Journal")
	TObjectPtr<UDataTable> JournalDatabase;
};
