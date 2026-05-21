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
class UInputConfigData;
class UUserWidget;

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

	/** Sprint Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	TObjectPtr<UInputAction> SprintAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	TObjectPtr<UInputAction> LookAction;

	/* End input actions */

	UPROPERTY(EditAnywhere, Category = "Input")
	UInputConfigData* InputConfig;

	UPROPERTY()
	TMap<UInputMappingContext*, int32> ActiveContextSnapshot;

	// Switches between normal and menu input mappings
	void SetMenuState(bool bIsMenuOpen, UUserWidget* InventoryWidgetInstance);

protected:
	/* Begin MappingContexts */
	
	/** Input Mapping Context to be used for simple input */
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

	/** Called for sprinting */
	void StartSprinting();
	void StopSprinting();
	bool bIsSprinting;
	float BaseWalkSpeed;
	float BaseWalkSpeedCrouched;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
	float SprintMultiplier = 2.0f;

	/* End Inputs */

	TObjectPtr<APlayerCharacter> PlayerCharacter;

	// Begin Actor interface
	virtual void BeginPlay() override;
	// End Actor interface

	virtual void SetupInputComponent() override;

};
