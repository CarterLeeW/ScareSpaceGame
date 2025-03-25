// Copyright Carter Wooton

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "MainPlayerController.generated.h"

class UInputMappingContext;
class APlayerCharacter;

/**
 * 
 */
UCLASS()
class SCARESPACE_API AMainPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	virtual void PlayerTick(float DeltaTime) override;

protected:
	/** Input Mapping Context to be used for player input */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	TObjectPtr<UInputMappingContext> InputMappingContext;

	TObjectPtr<APlayerCharacter> PlayerCharacter;

	// Begin Actor interface
	virtual void BeginPlay() override;
	// End Actor interface

private:
	void InteractionTrace();
};
