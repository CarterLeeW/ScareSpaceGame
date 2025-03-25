// Copyright Carter Wooton


#include "Controller/MainPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/LocalPlayer.h"
#include "Character/PlayerCharacter.h"
#include "Camera/CameraComponent.h"
#include "Interaction/Interactable.h"
#include "DrawDebugHelpers.h"

void AMainPlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);

	InteractionTrace();
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

void AMainPlayerController::InteractionTrace()
{
	FVector TraceStart = PlayerCharacter->FirstPersonCameraComponent->GetComponentLocation();
	FVector TraceEnd = TraceStart + (PlayerCharacter->FirstPersonCameraComponent->GetForwardVector() * PlayerCharacter->GetHandLength());

	FHitResult HitResult;
	GetWorld()->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, ECollisionChannel::ECC_Visibility);

	// We have a blocking hit, the actor is valid, and the actor implements the Interactable interface
	if (HitResult.bBlockingHit && IsValid(HitResult.GetActor()))
	{
		if (HitResult.GetActor()->Implements<UInteractable>())
		{
			DrawDebugSphere(GetWorld(), HitResult.Location, 10.0, 12, FColor::Blue);
		}
		else
		{
			DrawDebugSphere(GetWorld(), HitResult.Location, 10.0, 12, FColor::Red);
		}
	}
}
