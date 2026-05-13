// Copyright Carter Wooton


#include "Controller/MainPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/LocalPlayer.h"
#include "Character/PlayerCharacter.h"
#include "Camera/CameraComponent.h"
#include "Interaction/InteractableComponent.h"
#include "DrawDebugHelpers.h"
#include "EnhancedInputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Controller/InputConfigData.h"
#include "Blueprint/UserWidget.h"

void AMainPlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);

}

void AMainPlayerController::SetMenuState(bool bIsMenuOpen, UUserWidget* InventoryWidgetInstance)
{
	if (!InputConfig)
	{
		return;
	}
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		if (bIsMenuOpen)
		{
			// 1. Snapshot: Check which gameplay contexts are currently active
			ActiveContextSnapshot.Empty();
			for (UInputMappingContext* IMC : InputConfig->AllGameplayContexts)
			{
				int32 CurrentPriority = 0;
				if (Subsystem->HasMappingContext(IMC, CurrentPriority))
				{
					ActiveContextSnapshot.Add(IMC, CurrentPriority);
				}
			}
			// 2. Wipe all input
			Subsystem->ClearAllMappings();

			// 3. Add Menu Contexts
			for (UInputMappingContext* MenuIMC : InputConfig->MenuContexts)
			{
				Subsystem->AddMappingContext(MenuIMC, 10);
			}
			// 4. Input Mode
			FInputModeGameAndUI Mode;
			if (InventoryWidgetInstance)
			{
				Mode.SetWidgetToFocus(InventoryWidgetInstance->TakeWidget());
			}
			SetInputMode(Mode);
			InventoryWidgetInstance->SetKeyboardFocus();
			Mode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
			bShowMouseCursor = true;
		}
		else
		{
			// 1. Wipe Menu input
			Subsystem->ClearAllMappings();

			// 2. Restore only what was active before
			for (const TPair<UInputMappingContext*, int32> Pair : ActiveContextSnapshot)
			{
				Subsystem->AddMappingContext(Pair.Key, Pair.Value);
			}
			ActiveContextSnapshot.Empty();

			// 3. Return to Gameplay
			SetInputMode(FInputModeGameOnly());
			bShowMouseCursor = false;
		}
	}
}

void AMainPlayerController::Jump()
{
	PlayerCharacter->Jump();
}

void AMainPlayerController::StopJumping()
{
	PlayerCharacter->StopJumping();
}

void AMainPlayerController::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();
	
	PlayerCharacter->AddMovementInput(PlayerCharacter->GetActorForwardVector(), MovementVector.Y);
	PlayerCharacter->AddMovementInput(PlayerCharacter->GetActorRightVector(), MovementVector.X);
}

void AMainPlayerController::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	// add yaw and pitch input to controller
	PlayerCharacter->AddControllerYawInput(LookAxisVector.X);
	PlayerCharacter->AddControllerPitchInput(LookAxisVector.Y);
}

void AMainPlayerController::CrouchImplementation()
{
	if (PlayerCharacter->GetCharacterMovement() && PlayerCharacter->CanCrouch())
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(CrouchedMappingContext, 1);
			PlayerCharacter->GetCharacterMovement()->bWantsToCrouch = true;
		}
	}
}

void AMainPlayerController::UnCrouchImplementation()
{
	if (PlayerCharacter->GetCharacterMovement())
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
		{
			Subsystem->RemoveMappingContext(CrouchedMappingContext);
			PlayerCharacter->GetCharacterMovement()->bWantsToCrouch = false;
		}
	}
}

void AMainPlayerController::StartSprinting()
{
}

void AMainPlayerController::StopSprinting()
{
}

void AMainPlayerController::BeginPlay()
{
	Super::BeginPlay();

	// get the enhanced input subsystem
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		// add the mapping context so we get controls
		Subsystem->AddMappingContext(InputMappingContext, 0);
	}

	PlayerCharacter = Cast<APlayerCharacter>(GetCharacter());
}

void AMainPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent))
	{
		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &AMainPlayerController::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &AMainPlayerController::StopJumping);

		// Crouching
		EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Started, this, &AMainPlayerController::CrouchImplementation);
		EnhancedInputComponent->BindAction(UnCrouchAction, ETriggerEvent::Started, this, &AMainPlayerController::UnCrouchImplementation);

		// Sprinting
		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Started, this, &AMainPlayerController::StartSprinting);
		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Completed, this, &AMainPlayerController::StopSprinting);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AMainPlayerController::Move);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AMainPlayerController::Look);

	}
}
