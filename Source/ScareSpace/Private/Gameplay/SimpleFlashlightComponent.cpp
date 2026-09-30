// Copyright Carter Wooton


#include "Gameplay/SimpleFlashlightComponent.h"
#include "GameFramework/Character.h"
#include "Controller/MainPlayerController.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Kismet/GameplayStatics.h"

USimpleFlashlightComponent::USimpleFlashlightComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIntensity(BaseIntensity);
}

void USimpleFlashlightComponent::ToggleFlashlight()
{
	if (!bCanUse) return;
	if (bIsOn) // turn off
	{
		if (FlashlightOffSound)
		{
			UGameplayStatics::PlaySound2D(GetWorld(), FlashlightOffSound);
		}
		SetIntensity(0.0f);
		bIsOn = false;
	}
	else // turn on
	{
		if (FlashlightOnSound)
		{
			UGameplayStatics::PlaySound2D(GetWorld(), FlashlightOnSound);
		}
		SetIntensity(BaseIntensity);
		bIsOn = true;
	}
}

void USimpleFlashlightComponent::BeginPlay()
{
	Super::BeginPlay();

	ACharacter* ThisChar = Cast<ACharacter>(GetOwner());
	checkf(ThisChar, TEXT("SimpleFlashlightComponent must be attached to a Character!"));
	AMainPlayerController* ThisController = Cast<AMainPlayerController>(ThisChar->GetController());
	checkf(ThisController, TEXT("SimpleFlashlightComponent must be attached to a Character with a Player Controller!"));

	// Bind input actions to the controller
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
		ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(ThisController->GetLocalPlayer()))
	{
		Subsystem->AddMappingContext(FlashlightMappingContext, 0);
	}
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(ThisController->InputComponent))
	{
		EnhancedInputComponent->BindAction(ToggleFlashlightAction, ETriggerEvent::Started, this, &USimpleFlashlightComponent::ToggleFlashlight);
	}

	// Default state should be off when loading game
	SetIntensity(0.0f);
	bIsOn = false;
}
