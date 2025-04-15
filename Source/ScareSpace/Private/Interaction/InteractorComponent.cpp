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
	APlayerController* ThisController = Cast<APlayerController>(ThisChar->GetController());
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
			CurrentInteractableComponent->EndInteraction();
		}
		UE_LOG(LogTemp, Display, TEXT("End interaction reached from AMainPlayerController::RequestEndInteraction"));

		// Interaction is now over for sure
		bIsInteracting = false;
		CurrentInteractableComponent = nullptr;
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
