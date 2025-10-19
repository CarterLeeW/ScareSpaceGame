// Copyright Carter Wooton


#include "Inventory/InventoryComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/PlayerController.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
UInventoryComponent::UInventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}


void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();
	ACharacter* ThisChar = Cast<ACharacter>(GetOwner());
	checkf(ThisChar, TEXT("InventoryComponent must be attached to a Character!"));
	ThisController = Cast<APlayerController>(ThisChar->GetController());
	checkf(ThisController, TEXT("InventoryComponent must be attached to a Character with a PlayerController!"));
	// Bind Interactor input actions to the controller
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
		ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(ThisController->GetLocalPlayer()))
	{
		Subsystem->AddMappingContext(InventoryGameplayContext, 0);
	}
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(ThisController->InputComponent))
	{
		// Interaction
		EnhancedInputComponent->BindAction(OpenInventoryMenuAction, ETriggerEvent::Started, this, &UInventoryComponent::OpenInventoryMenu);
	}

}

void UInventoryComponent::OpenInventoryMenu()
{
	UE_LOG(LogTemp, Display, TEXT("Open Inventory"));
}
