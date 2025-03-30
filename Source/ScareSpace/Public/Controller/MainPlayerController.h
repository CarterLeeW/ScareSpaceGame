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
	/** 
	* Performs a single trace along the visibility channel for objects that are within arms length of the character
	* @param OutHit
	*/
	void ArmsLengthTrace(FHitResult& OutResult);

	// The result of the ArmsLengthTrace(). Can be further evaluated
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess=true))
	FHitResult ReachableTarget;
};
