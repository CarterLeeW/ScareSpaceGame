// Copyright Carter Wooton


#include "Journal/JournalComponent.h"
#include "GameFramework/Character.h"
#include "Controller/MainPlayerController.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Logging/ScareSpaceLogs.h"
#include "Blueprint/UserWidget.h"
#include "ItemStructs.h"

UJournalComponent::UJournalComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

}


void UJournalComponent::ToggleJournalMenu()
{
	if (!ThisController) return;

	if (!bIsJournalOpen && ThisController->bIsInMenuState)
	{
		UE_LOG(LogUI, Warning, TEXT("Cannot open journal menu because another menu is already open"));
		return;
	}

	bIsJournalOpen = !bIsJournalOpen;

	// Open the journal menu
	if (bIsJournalOpen)
	{
		// Create widget if not already created
		if (!IsValid(JournalWidgetInstance) && JournalWidgetClass)
		{
			UE_LOG(LogUI, Warning, TEXT("Journal widget instance was not valid before opening"));
			JournalWidgetInstance = CreateWidget<UUserWidget>(ThisController, JournalWidgetClass);
		}
		if (IsValid(JournalWidgetInstance))
		{
			// Show the UI
			JournalWidgetInstance->SetIsFocusable(true);
			JournalWidgetInstance->AddToViewport();
			// Tell Controller to use UI input mode
			ThisController->SetMenuState(true, JournalWidgetInstance);
		}
	}
	// Close the journal menu
	else
	{
		// Hide UI
		if (IsValid(JournalWidgetInstance))
		{
			JournalWidgetInstance->RemoveFromParent();
		}

		// Tell Controller to restore gameplay
		ThisController->SetMenuState(false, nullptr);
	}
}

bool UJournalComponent::AddItemToJournal(FDataTableRowHandle CollectableItemRow)
{
	// All journal items are unique
	if (JournalItemDatabase)
	{
		// item in database?
		if (FItemDataJournal* FoundItem = CollectableItemRow.GetRow<FItemDataJournal>(TEXT("JournalComponent add item to journal")))
		{
			if (!JournalItems.Contains(CollectableItemRow))
			{
				JournalItems.Add(CollectableItemRow);
				OnJournalUpdated.Broadcast(JournalItems);
				return true;
			}
			else
			{
				//UE_LOG(LogJournal, Warning, TEXT("Item %s already exists in journal"), *CollectableItemRow.RowName.ToString());
			}
		}
	}
	return false;
}

bool UJournalComponent::RemoveItemFromJournal(FDataTableRowHandle CollectableItemRow)
{
	if (JournalItems.Contains(CollectableItemRow))
	{
		JournalItems.Remove(CollectableItemRow);
		OnJournalUpdated.Broadcast(JournalItems);
		return true;
	}
	else
	{
		//UE_LOG(LogJournal, Warning, TEXT("Cannot remove %s from journal because it is not present"), *CollectableItemRow.RowName.ToString());
	}
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
