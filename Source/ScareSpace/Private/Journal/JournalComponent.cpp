// Copyright Carter Wooton


#include "Journal/JournalComponent.h"
#include "GameFramework/Character.h"
#include "Controller/MainPlayerController.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Logging/ScareSpaceLogs.h"
#include "Blueprint/UserWidget.h"

UJournalComponent::UJournalComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

}


void UJournalComponent::ToggleJournalMenu()
{
}

bool UJournalComponent::AddItemToJournal(FDataTableRowHandle CollectableItemRow)
{
	return false;
}

bool UJournalComponent::RemoveItemFromJournal(FDataTableRowHandle CollectableItemRow)
{
	return false;
}

void UJournalComponent::BeginPlay()
{
	Super::BeginPlay();

	checkf(JournalItemDatabase, TEXT("JournalItemDatabase is not set on JournalComponent of %s"), *GetOwner()->GetName());

	ACharacter* ThisChar = Cast<ACharacter>(GetOwner());
	checkf(ThisChar, TEXT("InventoryComponent must be attached to a Character!"));
	ThisController = Cast<AMainPlayerController>(ThisChar->GetController());
	checkf(ThisController, TEXT("InventoryComponent must be attached to a Character with a Player Controller!"));

	// Bind input actions to the controller
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
		ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(ThisController->GetLocalPlayer()))
	{
		Subsystem->AddMappingContext(JournalGameplayContext, 0);
	}
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(ThisController->InputComponent))
	{
		EnhancedInputComponent->BindAction(ToggleJournalMenuAction, ETriggerEvent::Started, this, &UJournalComponent::ToggleJournalMenu);
	}

	// Create the journal widget
	JournalWidgetInstance = CreateWidget<UUserWidget>(GetWorld(), JournalWidgetClass);
}

void UJournalComponent::OpenJournalMenu()
{
}

void UJournalComponent::CloseJournalMenu()
{
}
