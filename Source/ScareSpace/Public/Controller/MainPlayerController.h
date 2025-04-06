// Copyright Carter Wooton

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "MainPlayerController.generated.h"

class UInputAction;
class UInputMappingContext;
class APlayerCharacter;
struct FInputActionValue;

/**
 * 
 */
UCLASS()
class SCARESPACE_API AMainPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	virtual void PlayerTick(float DeltaTime) override;

	/* Begin input actions */

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* MoveAction;

	/** Crouch Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* CrouchAction;

	/** UnCrouch Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* UnCrouchAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	class UInputAction* LookAction;

	/* Interaction Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	class UInputAction* InteractAction;
	/* End input actions */

protected:
	/* Begin MappingContexts */

	/** Input Mapping Context to be used for player input */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	TObjectPtr<UInputMappingContext> InputMappingContext;

	/** Crouched MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	class UInputMappingContext* CrouchedMappingContext;

	/* End MappingContexts */

	/* Begin Inputs */

	/** Called for jump input using Character::Jump*/
	void Jump();
	void StopJumping();

	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);

	/** Called for crouching input */
	void CrouchImplementation();
	void UnCrouchImplementation();

	/* For interaction input */
	void BeginInteraction();
	void EndInteraction();

	/* End Inputs */

	TObjectPtr<APlayerCharacter> PlayerCharacter;

	// Begin Actor interface
	virtual void BeginPlay() override;
	// End Actor interface

	virtual void SetupInputComponent() override;

private:
	/** 
	* Performs a single trace along the visibility channel for objects that are within arms length of the character
	* @param OutHit
	*/
	void ArmsLengthTrace(FHitResult& OutResult);

	// The result of the ArmsLengthTrace(). Can be further evaluated
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess=true))
	FHitResult ReachableTargetHitResult;

};
