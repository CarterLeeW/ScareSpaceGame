// Copyright Carter Wooton


#include "Journal/JournalComponent.h"
#include "GameFramework/Character.h"
#include "Controller/MainPlayerController.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Logging/ScareSpaceLogs.h"
#include "Blueprint/UserWidget.h"
#include "ItemStructs.h"
#include "Core/LinearStoryProgressionSubsystem.h"


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
				UE_LOG(LogJournal, Warning, TEXT("Item %s already exists in journal"), *CollectableItemRow.RowName.ToString());
			}
		}
	}
	return false;
}

void UJournalComponent::HandleJournalEntryAddRequested(FDataTableRowHandle CollectableItemRow)
{
	// check that we don't already have this journal item
	if (JournalItems.Contains(CollectableItemRow))
	{
		UE_LOG(LogJournal, Log, TEXT("%s already in player's journal"), *CollectableItemRow.RowName.ToString());
		return;
	}
	if (AddItemToJournal(CollectableItemRow))
	{
		UE_LOG(LogJournal, Log, TEXT("Added %s to journal via subsystem request"), *CollectableItemRow.RowName.ToString());
	}
	else
	{
		UE_LOG(LogJournal, Warning, TEXT("Failed to add %s to journal via subsystem request"), *CollectableItemRow.RowName.ToString());
	}
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
		UE_LOG(LogJournal, Warning, TEXT("Cannot remove %s from journal because it is not present"), *CollectableItemRow.RowName.ToString());
	}
	return false;
}

bool UJournalComponent::ClearMementosFromJournal()
{
	// Remove all items that are mementos
	for (int32 i = JournalItems.Num() - 1; i >= 0; --i)
	{
		FDataTableRowHandle ItemRow = JournalItems[i];
		if (FItemDataJournal* FoundItem = ItemRow.GetRow<FItemDataJournal>(TEXT("JournalComponent clear mementos from journal")))
		{
			if (FoundItem->JournalItemSubtype == EJournalItemType::Memento)
			{
				JournalItems.RemoveAt(i);
			}
		}
	}
	return false;
}

void UJournalComponent::HandleJournalEntryRemoveRequested(FDataTableRowHandle CollectableItemRow)
{
	// check that we have this journal item
	if (!JournalItems.Contains(CollectableItemRow))
	{
		UE_LOG(LogJournal, Log, TEXT("%s not in player's journal"), *CollectableItemRow.RowName.ToString());
		return;
	}
	if (RemoveItemFromJournal(CollectableItemRow))
	{
		UE_LOG(LogJournal, Log, TEXT("Removed %s to journal via subsystem request"), *CollectableItemRow.RowName.ToString());
	}
	else
	{
		UE_LOG(LogJournal, Warning, TEXT("Failed to remove %s to journal via subsystem request"), *CollectableItemRow.RowName.ToString());
	}
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

	// Bind to subsystem updates
	if (UGameInstance* GameInstance = GetWorld()->GetGameInstance())
	{
		if (ULinearStoryProgressionSubsystem* Subsystem = GameInstance->GetSubsystem<ULinearStoryProgressionSubsystem>())
		{
			Subsystem->OnJournalItemAddRequested.AddDynamic(this, &UJournalComponent::HandleJournalEntryAddRequested);
			Subsystem->OnJournalItemRemoveRequested.AddDynamic(this, &UJournalComponent::HandleJournalEntryRemoveRequested);
		}
	}
}
