// Copyright Carter Wooton

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "MainGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class SCARESPACE_API UMainGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:

	// Updates or adds a story flag
	UFUNCTION(BlueprintCallable, Category = "Story")
	void SetStoryFlag(FName FlagName, bool bIsComplete);

	// Retrieves a story flag state, defaults to false if not found
	UFUNCTION(BlueprintPure, Category = "Story")
	bool GetStoryFlag(FName FlagName) const;

protected:

	// Stores the state of all boolean story flags
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Story")
	TMap<FName, bool> StoryFlags;
	
};
