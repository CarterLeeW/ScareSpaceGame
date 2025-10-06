// Copyright Carter Wooton


#include "Interaction/InteractorComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/PlayerController.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "PhysicsEngine/PhysicsHandleComponent.h"
#include "Interaction/InteractableComponent.h"

UInteractorComponent::UInteractorComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	/*
	* Ideally, this component will be attached to the FirstPersonCameraComponent and 
	* with a local location of (0,0,0)
	*/
}

void UInteractorComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	ArmsLengthTrace(ReachableTargetHitResult);

}


void UInteractorComponent::BeginPlay()
{
	Super::BeginPlay();
	ACharacter* ThisChar = Cast<ACharacter>(GetOwner());
	checkf(ThisChar, TEXT("InteractorComponent must be attached to a Character!"));
	ThisController = Cast<APlayerController>(ThisChar->GetController());
	checkf(ThisController, TEXT("InteractorComponent must be attached to a Character with a PlayerController!"));
	// Bind Interactor input actions to the controller
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
		ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(ThisController->GetLocalPlayer()))
	{
		Subsystem->AddMappingContext(InteractorMappingContext, 0);
	}
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(ThisController->InputComponent))
	{
		// Interaction
		EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Started, this, &UInteractorComponent::BeginInteraction);
		EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Completed, this, &UInteractorComponent::RequestEndInteraction);
		EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Triggered, this, &UInteractorComponent::ContinueInteraction);
		EnhancedInputComponent->BindAction(ThrowAction, ETriggerEvent::Triggered, this, &UInteractorComponent::ThrowObject);
	}

	// Store physics handle for use in interactions
	PhysicsHandle = GetOwner()->FindComponentByClass<UPhysicsHandleComponent>();
	checkf(PhysicsHandle, TEXT("Physics Handle cannot be found on parent component - From Interactor Component"))

}

void UInteractorComponent::BeginInteraction()
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
		CurrentInteractableComponent->BeginInteraction();

		/* Code related to input mappings and type of interaction */
		switch (CurrentInteractableComponent->InteractableType)
		{
		case EInteractableType::Holdable:
			BeginHolding();
			break;
		case EInteractableType::Collectable:

			break;
		default:
			UE_LOG(LogTemp, Warning, TEXT("EInteractableType cannot be found"));
		}

	}
	else
	{
		UE_LOG(LogTemp, Display, TEXT("No interactable actor"));
	}
}

void UInteractorComponent::RequestEndInteraction()
{
	// TODO: what if the interactable object becomes void before its InteractionEnded delegate is broadcast?
	if (bIsInteracting)
	{
		// Will be null if the component has been destroyed i.e. a collectible
		if (IsValid(CurrentInteractableComponent))
		{
			// If the object was holdable, release it from the physics handle.
			if (CurrentInteractableComponent->InteractableType == EInteractableType::Holdable)
			{
				// If object has already been thrown, then this has already occurred
				if (PhysicsHandle && PhysicsHandle->GetGrabbedComponent())
				{
					UE_LOG(LogTemp, Display, TEXT("object dropped without throwing"));
					PhysicsHandle->ReleaseComponent();
				}
			}
			// Check the object's velocity so you can't shoot it to the moon
			FVector PlayerVelocity = GetOwner()->GetVelocity();
			// End the interaction on the interactable component
			CurrentInteractableComponent->EndInteraction();
		}
		// UE_LOG(LogTemp, Display, TEXT("End interaction reached from AMainPlayerController::RequestEndInteraction"));
		// Interaction is now over for sure

		/* Code related to input mappings */

		CurrentInteractableComponent = nullptr;
		bIsInteracting = false;
	}
	// Remove the holding mapping context
	checkf(ThisController, TEXT("PlayerController cannot be found when holding."));
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(ThisController->GetLocalPlayer()))
	{
		Subsystem->RemoveMappingContext(HoldingMappingContext);
	}
}

void UInteractorComponent::ContinueInteraction()
{
	// We don't need to continue in this case
	if (!bIsInteracting)
	{
		return;
	}
	/* Check what type of interaction it is,
	* if holding, then do physics handle things,
	* if pivoting, then .....
	*/
	switch (CurrentInteractableComponent->InteractableType)
	{
	case EInteractableType::Holdable:
		ContinueHolding();
		break;
	case EInteractableType::Collectable:

		break;
	default:
		UE_LOG(LogTemp, Warning, TEXT("EInteractableType cannot be found"));
	}

	//UE_LOG(LogTemp, Display, TEXT("Continuing interaction"));
}

void UInteractorComponent::ThrowObject()
{
	if (PhysicsHandle && PhysicsHandle->GetGrabbedComponent())
	{
		// Get the component to throw
		UPrimitiveComponent* ComponentToThrow = PhysicsHandle->GetGrabbedComponent();
		if (!IsValid(ComponentToThrow))
		{
			UE_LOG(LogTemp, Warning, TEXT("Component to throw is not valid!"));
			return;
		}
		// Set the component to simulate physics and wake it up
		ComponentToThrow->SetSimulatePhysics(true);
		ComponentToThrow->WakeAllRigidBodies();
		// Calculate the throw velocity based on the current held length and forward vector
		// Add a slight vertical increase to the throw angle
		FVector Forward = GetForwardVector();
		FVector Up = FVector::UpVector;
		float VerticalBoost = 0.2f; // Adjust this value for more/less vertical boost
		FVector ThrowDirection = (Forward + Up * VerticalBoost).GetSafeNormal();

		FVector ThrowVelocity = ThrowDirection * TargetHoldLength * ThrowForceMultiplier;
		ComponentToThrow->AddImpulse(ThrowVelocity, NAME_None, true);

		PhysicsHandle->ReleaseComponent();
		RequestEndInteraction();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("No component is currently being held!"));
	}
}

void UInteractorComponent::ArmsLengthTrace(FHitResult& OutResult)
{
	FVector TraceStart = GetComponentLocation();
	FVector TraceEnd = TraceStart + (GetForwardVector() * HandLength);

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

void UInteractorComponent::BeginHolding()
{
	UPrimitiveComponent* ComponentToHold = ReachableTargetHitResult.GetComponent();
	if (!IsValid(ComponentToHold))
	{
		UE_LOG(LogTemp, Warning, TEXT("Component to hold is not valid!"));
		return;
	}
	ComponentToHold->SetSimulatePhysics(true);
	ComponentToHold->WakeAllRigidBodies();
	// --Uses actor's root component-- should this change??
	TargetHoldLength = FVector::Dist(GetComponentLocation(), ReachableTargetHitResult.ImpactPoint);
	FVector TargetLocation = GetComponentLocation() + (GetForwardVector() * TargetHoldLength);
	PhysicsHandle->SetTargetLocationAndRotation(TargetLocation, GetComponentRotation());

	// Give player the holding controls mapping context
	checkf(ThisController, TEXT("PlayerController cannot be found when holding."));
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(ThisController->GetLocalPlayer()))
	{
		Subsystem->AddMappingContext(HoldingMappingContext, 1);
	}
	

	// NOTE: To avoid collision issues, the component and the character should not block each other.
	// HOWERVER, they should overlap, so that you can only drop the object when you are not inside it.
	// This can be done on the HoldableComponent itself
	// TODO: When the object is dropped, it clips out of the character and may push the character around, but it is not smooth

	// May need to change these parameters depending on the behavior
	PhysicsHandle->GrabComponentAtLocationWithRotation(
		ComponentToHold,
		NAME_None,
		ReachableTargetHitResult.ImpactPoint,
		GetComponentRotation()
	);
}

/**
 * This will be called every frame while the object is being held, so be careful with performance.
 */
void UInteractorComponent::ContinueHolding()
{
	// If the distance between the player and the held object is greater than the HoldAutoDropDistance, then drop it
	if (CurrentInteractableComponent)
	{
		float CurrentHeldLength = FVector::Dist(GetComponentLocation(), CurrentInteractableComponent->GetOwner()->GetActorLocation());
		// UE_LOG(LogTemp, Display, TEXT("current held length: %f"), CurrentHeldLength);
		if (CurrentHeldLength > HoldAutoDropDistance)
		{
			RequestEndInteraction();
			return;
		}
	}
	// UE_LOG(LogTemp, Display, TEXT("target hold length: %f"), TargetHoldLength);
	FVector TargetLocation = GetComponentLocation() + (GetForwardVector() * TargetHoldLength);
	PhysicsHandle->SetTargetLocationAndRotation(TargetLocation, GetComponentRotation());
}
