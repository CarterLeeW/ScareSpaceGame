// Copyright Carter Wooton

#include "Interaction/InteractorComponent.h"
#include "Interaction/InteractableComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/PlayerController.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "PhysicsEngine/PhysicsHandleComponent.h"
#include "Logging/ScareSpaceLogs.h"
#include "Inventory/InventoryComponent.h"
#include "Journal/JournalComponent.h"
#include "InputActionValue.h"
#include "ItemStructs.h"

UInteractorComponent::UInteractorComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UInteractorComponent::BeginPlay()
{
	Super::BeginPlay();

	ThisCharacter = Cast<ACharacter>(GetOwner());
	checkf(ThisCharacter, TEXT("InteractorComponent must be attached to a Character!"));

	ThisController = Cast<APlayerController>(ThisCharacter->GetController());
	checkf(ThisController, TEXT("InteractorComponent must be attached to a Character with a PlayerController!"));

	InventoryComponent = ThisCharacter->FindComponentByClass<UInventoryComponent>();
	checkf(InventoryComponent, TEXT("InteractorComponent requires an InventoryComponent on the Character!"));
	InventoryComponent->OnItemSelected.AddDynamic(this, &UInteractorComponent::HandleOnItemSelected);

	JournalComponent = ThisCharacter->FindComponentByClass<UJournalComponent>();
	PhysicsHandle = GetOwner()->FindComponentByClass<UPhysicsHandleComponent>();
	checkf(PhysicsHandle, TEXT("Physics Handle cannot be found on parent component - From Interactor Component"));

	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(ThisController->GetLocalPlayer()))
	{
		Subsystem->AddMappingContext(InteractorMappingContext, 0);
	}

	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(ThisController->InputComponent))
	{
		EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Started, this, &UInteractorComponent::BeginInteraction);
		EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Completed, this, &UInteractorComponent::RequestEndInteraction);
		EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Triggered, this, &UInteractorComponent::ContinueInteraction);
		EnhancedInputComponent->BindAction(ThrowAction, ETriggerEvent::Triggered, this, &UInteractorComponent::ThrowObject);
		EnhancedInputComponent->BindAction(PivotAction, ETriggerEvent::Triggered, this, &UInteractorComponent::ProcessLateralOffset);
		EnhancedInputComponent->BindAction(DropHeldItemAction, ETriggerEvent::Triggered, this, &UInteractorComponent::StopHoldingItem);
	}
}

void UInteractorComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!bIsInteracting)
	{
		ArmsLengthTrace();
	}
	UpdateInteractionPrompt();
}

void UInteractorComponent::ArmsLengthTrace()
{
	FVector TraceStart = GetComponentLocation();
	FVector TraceEnd = TraceStart + (GetForwardVector() * HandLength);

	GetWorld()->LineTraceSingleByChannel(ReachableTargetHitResult, TraceStart, TraceEnd, ECC_Visibility);

	UInteractableComponent* ResolvedInteractable = nullptr;
	if (ReachableTargetHitResult.bBlockingHit && IsValid(ReachableTargetHitResult.GetComponent()))
	{
		ResolvedInteractable = FindInteractableForMesh(ReachableTargetHitResult.GetComponent());
	}

	HoveredInteractableComponent = ResolvedInteractable;
}

UInteractableComponent* UInteractorComponent::FindInteractableForMesh(UPrimitiveComponent* HitMesh) const
{
	if (!IsValid(HitMesh) || !IsValid(HitMesh->GetOwner()))
	{
		return nullptr;
	}

	TArray<UInteractableComponent*> Interactables;
	HitMesh->GetOwner()->GetComponents<UInteractableComponent>(Interactables);

	for (UInteractableComponent* Interactable : Interactables)
	{
		if (IsValid(Interactable) && Interactable->IsBoundToMesh(HitMesh))
		{
			return Interactable;
		}
	}

	
	return nullptr;
}

void UInteractorComponent::BeginInteraction()
{
	if (bIsInteracting)
	{
		return;
	}

	if (!ReachableTargetHitResult.bBlockingHit || !HoveredInteractableComponent.IsValid())
	{
		if (bHoldingItem)
		{
			StopHoldingItem();
		}
		return;
	}

	UInteractableComponent* Target = HoveredInteractableComponent.Get();

	if (bHoldingItem)
	{
		if (Target->QuickValidateItemInteraction(ActiveHeldItemRow))
		{
			if (Target->TryInteractWithItem(ActiveHeldItemRow, this))
			{
				InventoryComponent->ConsumeItem(ActiveHeldItemRow);
				StopHoldingItem();
			}
			return;
		}
		StopHoldingItem();
		return;
	}

	if (Target->BeginInteraction(this))
	{
		CurrentInteractableComponent = Target;
		bIsInteracting = true;
	}
}

void UInteractorComponent::ContinueInteraction()
{
	if (!bIsInteracting || !CurrentInteractableComponent.IsValid())
	{
		return;
	}

	CurrentInteractableComponent->ContinueInteraction(this);
}

void UInteractorComponent::RequestEndInteraction()
{
	if (bIsInteracting)
	{
		if (CurrentInteractableComponent.IsValid())
		{
			CurrentInteractableComponent->EndInteraction(this);
		}

		CurrentInteractableComponent.Reset();
		bIsInteracting = false;
	}

	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(ThisController->GetLocalPlayer()))
	{
		if (HoldingObjectMappingContext)
		{
			Subsystem->RemoveMappingContext(HoldingObjectMappingContext);
		}
		if (PivotingMappingContext)
		{
			Subsystem->RemoveMappingContext(PivotingMappingContext);
		}
	}
}

void UInteractorComponent::ThrowObject()
{
	if (bIsInteracting && CurrentInteractableComponent.IsValid())
	{
		CurrentInteractableComponent->OnThrow(this);

		RequestEndInteraction();
	}
}

void UInteractorComponent::ProcessLateralOffset(const FInputActionValue& Value)
{
	if (bIsInteracting && CurrentInteractableComponent.IsValid())
	{
		CurrentInteractableComponent->ProcessInputDelta(Value.Get<FVector2D>(), this);
	}
}

void UInteractorComponent::HandleOnItemSelected(FDataTableRowHandle SelectedItemRow)
{
	ActiveHeldItemRow = SelectedItemRow;
	CachedActiveItemIcon = nullptr;
	bHoldingItem = !ActiveHeldItemRow.IsNull();

	if (bHoldingItem)
	{
		if (FItemDataInventory* RowData = ActiveHeldItemRow.GetRow<FItemDataInventory>(TEXT("Interactor Context")))
		{
			CachedActiveItemIcon = RowData->ItemIcon;
		}

		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(ThisController->GetLocalPlayer()))
		{
			if (HoldingItemMappingContext)
			{
				Subsystem->AddMappingContext(HoldingItemMappingContext, 5);
			}
		}
	}

	UpdateInteractionPrompt();
}

void UInteractorComponent::StopHoldingItem()
{
	bHoldingItem = false;
	ActiveHeldItemRow = FDataTableRowHandle();
	CachedActiveItemIcon = nullptr;

	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(ThisController->GetLocalPlayer()))
	{
		if (HoldingItemMappingContext)
		{
			Subsystem->RemoveMappingContext(HoldingItemMappingContext);
		}
	}
}

void UInteractorComponent::UpdateInteractionPrompt()
{
	UTexture2D* CurrentIcon = nullptr;
	bool bIsTargetValid = false;

	if (bHoldingItem && CachedActiveItemIcon)
	{
		// Always show the item icon if we are holding it
		CurrentIcon = CachedActiveItemIcon;

		// Only validate the target if the component explicitly authorizes this specific item row
		if (HoveredInteractableComponent.IsValid() && HoveredInteractableComponent->QuickValidateItemInteraction(ActiveHeldItemRow))
		{
			bIsTargetValid = true;
		}
	}
	else if (!bIsInteracting && HoveredInteractableComponent.IsValid())
	{
		// Standard interaction (no item held)
		CurrentIcon = HoveredInteractableComponent->GetInteractionIcon(this);
		bIsTargetValid = true;
	}

	// Only broadcast to the UI if the icon OR the validation state has changed
	if (CurrentIcon != LastInteractionIcon.Get() || bIsTargetValid != bLastTargetValid)
	{
		LastInteractionIcon = CurrentIcon;
		bLastTargetValid = bIsTargetValid;
		OnInteractionIconChanged.Broadcast(CurrentIcon, bIsTargetValid);
	}
}
