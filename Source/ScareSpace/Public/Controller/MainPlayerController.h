// Copyright Carter Wooton

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "MainPlayerController.generated.h"

class UInputAction;
class UInputMappingContext;
class APlayerCharacter;
struct FInputActionValue;
class UInteractableComponent;

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
	TObjectPtr<UInputAction> JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	TObjectPtr<UInputAction> MoveAction;

	/** Crouch Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	TObjectPtr<UInputAction> CrouchAction;

	/** UnCrouch Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	TObjectPtr<UInputAction> UnCrouchAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	TObjectPtr<UInputAction> LookAction;

	/* Interaction Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	TObjectPtr<UInputAction> InteractAction;
	/* End input actions */

protected:
	/* Begin MappingContexts */

	/** Input Mapping Context to be used for player input */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	TObjectPtr<UInputMappingContext> InputMappingContext;

	/** Crouched MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	TObjectPtr<UInputMappingContext> CrouchedMappingContext;

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
	/* For mouse release - requests the interaction to end */
	void RequestEndInteraction();
	/**
	* Maybe it is unneccessary for this to be part of the input action completed, 
	* since the interactable object will broadcast when its interaction is finished,
	* which will call this function anyway.
	*/
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

	// Used to determing if a mouse click/release may cause an interaction to occur
	bool bIsInteracting = false;

	// Component this controller is currently interracting with - will be nullptr if no interaction
	TObjectPtr<UInteractableComponent> CurrentInteractableComponent = nullptr;

};
