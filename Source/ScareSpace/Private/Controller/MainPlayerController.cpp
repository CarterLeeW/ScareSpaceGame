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

void AMainPlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);

	ArmsLengthTrace(ReachableTargetHitResult);
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

void AMainPlayerController::BeginInteraction()
{
	// No targets are in reach, OR player is currently interacting, so there can be no new interaction
	if (!ReachableTargetHitResult.bBlockingHit || bIsInteracting)
	{
		return;
	}

	// Will be nullptr if none exists
	CurrentInteractableComponent = ReachableTargetHitResult.GetActor()->GetComponentByClass<UInteractableComponent>();
	// The actor has an interactable component
	if (IsValid(CurrentInteractableComponent))
	{
		bIsInteracting = true;
		// The interactable object determines when the action is ended, which calls this controller's end function
		// TODO: This delegate functionality is currently unnecessary
		//CurrentInteractableComponent->InteractionEnded.AddUObject(this, &AMainPlayerController::EndInteraction);

		CurrentInteractableComponent->BeginInteraction();
	}
	else
	{
		UE_LOG(LogTemp, Display, TEXT("No interactable actor"));
	}
}

void AMainPlayerController::RequestEndInteraction()
{
	// TODO: what if the interactable object becomes void before its InteractionEnded delegate is broadcast?
	if (bIsInteracting)
	{
		// Will be null if the component has been destroyed i.e. a collectible
		if (IsValid(CurrentInteractableComponent))
		{
			CurrentInteractableComponent->EndInteraction();
		}
		UE_LOG(LogTemp, Display, TEXT("End interaction reached from AMainPlayerController::RequestEndInteraction"));

		// Interaction is now over for sure
		bIsInteracting = false;
		CurrentInteractableComponent = nullptr;
	}
}

void AMainPlayerController::EndInteraction()
{
	UE_LOG(LogTemp, Display, TEXT("End interaction reached from AMainPlayerController::EndInteraction"));
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

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AMainPlayerController::Move);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AMainPlayerController::Look);

	}
}

void AMainPlayerController::ArmsLengthTrace(FHitResult& OutResult)
{
	FVector TraceStart = PlayerCharacter->FirstPersonCameraComponent->GetComponentLocation();
	FVector TraceEnd = TraceStart + (PlayerCharacter->FirstPersonCameraComponent->GetForwardVector() * PlayerCharacter->GetHandLength());

	GetWorld()->LineTraceSingleByChannel(OutResult, TraceStart, TraceEnd, ECollisionChannel::ECC_Visibility);

	//// We have a blocking hit, the actor is valid, and the actor implements the Interactable interface
	//if (HitResult.bBlockingHit && IsValid(HitResult.GetActor()))
	//{
	//	if (HitResult.GetActor()->Implements<UInteractable>())
	//	{
	//		return HitResult.GetActor();
	//	}
	//	else
	//	{
	//		return nullptr;
	//	}
	//}
}
