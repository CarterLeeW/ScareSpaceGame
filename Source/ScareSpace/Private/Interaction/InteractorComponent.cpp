// Copyright Carter Wooton


#include "Interaction/InteractorComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/PlayerController.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"

UInteractorComponent::UInteractorComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

}

void UInteractorComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
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
}

void UInteractorComponent::BeginInteraction()
{
}

void UInteractorComponent::RequestEndInteraction()
{
}

void UInteractorComponent::ArmsLengthTrace(FHitResult& OutResult)
{
	FVector TraceStart = GetComponentLocation();
	FVector TraceEnd = TraceStart + (GetForwardVector() * PlayerCharacter->GetHandLength());

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
