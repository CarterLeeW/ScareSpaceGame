// Copyright Carter Wooton


#include "Interaction/InteractorComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/PlayerController.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "PhysicsEngine/PhysicsHandleComponent.h"
#include "Interaction/InteractableComponent.h"
#include "Logging/ScareSpaceLogs.h"
#include "Inventory/InventoryComponent.h"
#include "Interaction/CollectableComponent.h"
#include "Interaction/PivotableComponent.h"
#include "InputActionValue.h"
#include "ItemStructs.h"

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

	// Save a trace when we are already interacting
	if (!bIsInteracting)
	{
		ArmsLengthTrace(ReachableTargetHitResult);
	}
	UpdateInteractionPrompt();
}


void UInteractorComponent::BeginPlay()
{
	Super::BeginPlay();

	ThisCharacter = Cast<ACharacter>(GetOwner());
	checkf(ThisCharacter, TEXT("InteractorComponent must be attached to a Character!"));

	ThisController = Cast<APlayerController>(ThisCharacter->GetController());
	checkf(ThisController, TEXT("InteractorComponent must be attached to a Character with a PlayerController!"));

	// Discover and bind event for InventoryComponent on the character
	InventoryComponent = ThisCharacter->FindComponentByClass<UInventoryComponent>();
	checkf(InventoryComponent, TEXT("InteractorComponent requires an InventoryComponent on the Character!"));
	InventoryComponent->OnItemSelected.AddDynamic(this, &UInteractorComponent::HandleOnItemSelected);

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
		EnhancedInputComponent->BindAction(PushAction, ETriggerEvent::Triggered, this, &UInteractorComponent::PushObject);
		EnhancedInputComponent->BindAction(PivotAction, ETriggerEvent::Triggered, this, &UInteractorComponent::CalculateLateralOffset);
		EnhancedInputComponent->BindAction(DropHeldItemAction, ETriggerEvent::Triggered, this, &UInteractorComponent::StopHoldingItem);
	}

	// Store physics handle for use in interactions
	PhysicsHandle = GetOwner()->FindComponentByClass<UPhysicsHandleComponent>();
	checkf(PhysicsHandle, TEXT("Physics Handle cannot be found on parent component - From Interactor Component"))

}

void UInteractorComponent::BeginInteraction()
{

	if (!ReachableTargetHitResult.bBlockingHit || bIsInteracting)
	{
		// We are holding an item and click but there is no interactable component to use it on, so we should stop holding the item
		if (bHoldingItem)
		{
			StopHoldingItem();
			return;
		}
		return;
	}

	// 
	// We look for the interactable component that is associated with the mesh that was hit by the trace
	//
	CurrentInteractableComponent = nullptr;
	// Retrieve all interactable components on the target actor
	TArray<UInteractableComponent*> Interactables;
	ReachableTargetHitResult.GetActor()->GetComponents<UInteractableComponent>(Interactables);
	// Determine which specific component matches the traced mesh
	for (UInteractableComponent* Interactable : Interactables)
	{
		if (UPivotableComponent* Pivotable = Cast<UPivotableComponent>(Interactable))
		{
			// Verify if this pivotable component drives the exact mesh hit by the trace
			if (IsChildOfPivotableComponent(ReachableTargetHitResult.GetComponent(), Pivotable))
			{
				CurrentInteractableComponent = Pivotable;
				break;
			}
		}
		else
		{
			// Fallback for singular interactables like Holdables or Collectables
			CurrentInteractableComponent = Interactable;
			break;
		}
	}

	//
	// Now we decide whether to interact with the component with an item from the inventory or in the usual way
	// 
	// If we are holding an item from the inventory, then we want to interact with it using that item instead of in the usual way
	// This runs when clicking anywhere after selecting item from inventory
	if (bHoldingItem && IsValid(CurrentInteractableComponent))
	{
		// If the interactable component is valid, we can attempt to interact with it using the held item
		if (CurrentInteractableComponent->QuickValidateItemInteraction(ActiveHeldItemRow))
		{
			UE_LOG(LogInteraction, Display, TEXT("We found a match!"));
			if (CurrentInteractableComponent->TryInteractWithItem(ActiveHeldItemRow))
			{
				// Consume item
				InventoryComponent->ConsumeItem(ActiveHeldItemRow);
				StopHoldingItem();
			}
			return;
		}
		// Fallback
		// Do stuff like unholding the item or displaying a message like "nothing to interact with"
		UE_LOG(LogInteraction, Display, TEXT("No match! Should put item back or let the player know"));
		StopHoldingItem();
	}
	// We are not holding an item so we can interact with things normally
	else if (IsValid(CurrentInteractableComponent))
	{
		bIsInteracting = true;
		CurrentInteractableComponent->BeginInteraction();

		switch (CurrentInteractableComponent->InteractableType)
		{
		case EInteractableType::Holdable:
			BeginHoldingObject();
			break;
		case EInteractableType::Collectable:
			Collect();
			break;
		case EInteractableType::Pivotable:
			BeginPivoting();
			break;
		case EInteractableType::ItemOnly:
			UE_LOG(LogInteraction, Display, TEXT("%s has a basic item-only interactable component and requires an item to interact with"), *CurrentInteractableComponent->GetOwner()->GetName());
			break;
		default:
			UE_LOG(LogInteraction, Warning, TEXT("EInteractableType cannot be handled on %s"), *CurrentInteractableComponent->GetOwner()->GetName());
		}
	}
	else
	{
		UE_LOG(LogInteraction, Display, TEXT("No valid interactable component matched the hit mesh"));
	}
}

void UInteractorComponent::StopHoldingItem()
{
	bHoldingItem = false;
	ActiveHeldItemRow = FDataTableRowHandle();
	CachedActiveItemIcon = nullptr;
	// Remove gameplay mapping context for holding item
	checkf(ThisController, TEXT("PlayerController cannot be found when holding."));
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(ThisController->GetLocalPlayer()))
	{
		Subsystem->RemoveMappingContext(HoldingItemMappingContext);
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
			if (CurrentInteractableComponent->InteractableType == EInteractableType::Holdable || CurrentInteractableComponent->InteractableType == EInteractableType::Pivotable)
			{
				// If object has already been thrown, then this has already occurred
				if (PhysicsHandle && PhysicsHandle->GetGrabbedComponent())
				{
					PhysicsHandle->ReleaseComponent();
				}
			}
			// End the interaction on the interactable component
			CurrentInteractableComponent->EndInteraction();
		}
		// UE_LOG(LogInteraction, Display, TEXT("End interaction reached from AMainPlayerController::RequestEndInteraction"));
		// Interaction is now over for sure

		/* Code related to input mappings */

		CurrentInteractableComponent = nullptr;
		bIsInteracting = false;
	}
	// Remove the mapping contexts
	checkf(ThisController, TEXT("PlayerController cannot be found when holding."));
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(ThisController->GetLocalPlayer()))
	{
		Subsystem->RemoveMappingContext(HoldingObjectMappingContext);
		Subsystem->RemoveMappingContext(PivotingMappingContext);
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
		ContinueHoldingObject();
		break;
	case EInteractableType::Pivotable:
		ContinuePivoting();
		break;
	default:
		//UE_LOG(LogInteraction, Warning, TEXT("EInteractableType cannot be found on %s"), *CurrentInteractableComponent->GetOwner()->GetName());
		break;
	}

	//UE_LOG(LogInteraction, Display, TEXT("Continuing interaction"));
}

void UInteractorComponent::ThrowObject()
{
	if (PhysicsHandle && PhysicsHandle->GetGrabbedComponent())
	{
		// Get the component to throw
		UPrimitiveComponent* ComponentToThrow = PhysicsHandle->GetGrabbedComponent();
		if (!IsValid(ComponentToThrow))
		{
			UE_LOG(LogInteraction, Warning, TEXT("Component to throw is not valid!"));
			return;
		}
		float ObjectMass = ComponentToThrow->GetMass();
		// Set the component to simulate physics and wake it up
		ComponentToThrow->SetSimulatePhysics(true);
		ComponentToThrow->WakeAllRigidBodies();
		// Calculate the throw velocity based on the current held length and forward vector
		// Add a slight vertical increase to the throw angle
		FVector Forward = GetForwardVector();
		FVector Up = FVector::UpVector;
		float VerticalBoost = 0.2f; // Adjust this value for more/less vertical boost
		FVector ThrowDirection = (Forward + Up * VerticalBoost).GetSafeNormal();

		FVector ThrowVelocity = (ThrowDirection * TargetHoldLength * ThrowForceMultiplier) / ObjectMass;
		ComponentToThrow->AddImpulse(ThrowVelocity, NAME_None, true);

		PhysicsHandle->ReleaseComponent();
		RequestEndInteraction();
	}
	else
	{
		UE_LOG(LogInteraction, Warning, TEXT("No component is currently being held!"));
	}
}

void UInteractorComponent::PushObject()
{

}

void UInteractorComponent::HandleOnItemSelected(FDataTableRowHandle SelectedItemRow)
{
	ActiveHeldItemRow = SelectedItemRow;
	CachedActiveItemIcon = nullptr;
	bHoldingItem = !ActiveHeldItemRow.IsNull();

	if (bHoldingItem)
	{
		FItemData* RowData = ActiveHeldItemRow.GetRow<FItemData>(TEXT("Interactor Context"));
		checkf(RowData, TEXT("RowData is null for the selected item row! Maybe we deleted the row or the data table is invalid."));
		CachedActiveItemIcon = RowData->ItemIcon;
		// Add gameplay mapping context for holding item
		checkf(ThisController, TEXT("PlayerController cannot be found when holding."));
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(ThisController->GetLocalPlayer()))
		{
			// This mapping context is disappearing immediately after this function finishes for some reason
			Subsystem->AddMappingContext(HoldingItemMappingContext, 5);
		}
	}

	UpdateInteractionPrompt();
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

void UInteractorComponent::BeginHoldingObject()
{
	UPrimitiveComponent* ComponentToHold = ReachableTargetHitResult.GetComponent();
	if (!IsValid(ComponentToHold))
	{
		UE_LOG(LogInteraction, Warning, TEXT("Component to hold is not valid!"));
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
		Subsystem->AddMappingContext(HoldingObjectMappingContext, 5);
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
void UInteractorComponent::ContinueHoldingObject()
{
	// If the distance between the player and the held object is greater than the HoldAutoDropDistance, then drop it
	if (CurrentInteractableComponent)
	{
		float CurrentHeldLength = FVector::Dist(GetComponentLocation(), CurrentInteractableComponent->GetOwner()->GetActorLocation());
		// UE_LOG(LogInteraction, Display, TEXT("current held length: %f"), CurrentHeldLength);
		if (CurrentHeldLength > HoldAutoDropDistance)
		{
			RequestEndInteraction();
			return;
		}
	}
	// UE_LOG(LogInteraction, Display, TEXT("target hold length: %f"), TargetHoldLength);
	FVector TargetLocation = GetComponentLocation() + (GetForwardVector() * TargetHoldLength);
	PhysicsHandle->SetTargetLocationAndRotation(TargetLocation, GetComponentRotation());
}

void UInteractorComponent::BeginPivoting()
{
	UPrimitiveComponent* ComponentToHold = ReachableTargetHitResult.GetComponent();
	UPivotableComponent* PivotableComp = Cast<UPivotableComponent>(CurrentInteractableComponent);
	if (!IsValid(ComponentToHold) || !IsValid(PivotableComp))
	{
		UE_LOG(LogInteraction, Warning, TEXT("Component to hold is not valid!"));
		return;
	}

	// Give player the pivoting controls mapping context
	checkf(ThisController, TEXT("PlayerController cannot be found when holding."));
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(ThisController->GetLocalPlayer()))
	{
		Subsystem->AddMappingContext(PivotingMappingContext, 5);
	}

	// decide if the component is locked or not - if locked, do not allow pivoting
	if (PivotableComp->GetIsLocked())
	{
		UE_LOG(LogInteraction, Display, TEXT("Pivotable component is locked, cannot pivot."));
		return;
	}
	else
	{
		ComponentToHold->SetSimulatePhysics(true);
		ComponentToHold->WakeAllRigidBodies();

		TargetHoldLength = FVector::Dist(GetComponentLocation(), ReachableTargetHitResult.ImpactPoint);
		TargetSideLength = 0.0f; // Reset side offset so we start centered on the hand
		FVector TargetLocation = GetComponentLocation() + (GetForwardVector() * TargetHoldLength);
		PhysicsHandle->SetTargetLocationAndRotation(TargetLocation, GetComponentRotation());
		PhysicsHandle->GrabComponentAtLocationWithRotation(
			ComponentToHold,
			NAME_None,
			ReachableTargetHitResult.ImpactPoint,
			GetComponentRotation()
		);
	}
	// Fire cosmetics on the pivotable component
	PivotableComp->BeginInteraction();
}

void UInteractorComponent::ContinuePivoting()
{
	// If the distance between the player and the held object is greater than the HoldAutoDropDistance, then drop it
	if (CurrentInteractableComponent)
	{
		float CurrentHeldLength = FVector::Dist(GetComponentLocation(), CurrentInteractableComponent->GetOwner()->GetActorLocation());
		// UE_LOG(LogInteraction, Display, TEXT("current held length: %f"), CurrentHeldLength);
		if (CurrentHeldLength > HoldAutoDropDistance)
		{
			RequestEndInteraction();
			return;
		}
	}
	// C. Calculate Final Location
	FVector StartLoc = GetComponentLocation();

	// Forward Vector * Forward Distance
	FVector ForwardLoc = GetForwardVector() * TargetHoldLength;

	// Right Vector * Side Distance (Calculated Fresh Every Frame)
	FVector SideLoc = GetOwner()->GetActorRightVector() * TargetSideLength;

	// Combine them
	FVector TargetLocation = StartLoc + ForwardLoc + SideLoc;

	// D. Update Handle
	PhysicsHandle->SetTargetLocationAndRotation(TargetLocation, GetComponentRotation());
}

void UInteractorComponent::CalculateLateralOffset(const FInputActionValue& Value)
{
	FVector2D MouseDelta = Value.Get<FVector2D>();

	if (PhysicsHandle && PhysicsHandle->GetGrabbedComponent())
	{
		// SENSITIVITY SETTINGS
		float PushPullSensitivity = 2.0f;
		float LateralSensitivity = 2.0f;

		// A. Handle Push/Pull (Accumulate Length)
		TargetHoldLength += (MouseDelta.Y * PushPullSensitivity);
		// Clamp to prevent pulling through player or pushing too far
		TargetHoldLength = FMath::Clamp(TargetHoldLength, 50.0f, 300.0f);

		// B. Handle Swing (Accumulate Side Distance)
		// We use += here so the door stays to the side when we stop moving the mouse
		TargetSideLength += (MouseDelta.X * LateralSensitivity);

		// Optional: Clamp side movement if you don't want them spinning 360 around the player
		TargetSideLength = FMath::Clamp(TargetSideLength, -200.0f, 200.0f);
	}
}

void UInteractorComponent::Collect()
{
	// Get inventory component from the owner
	if (ThisCharacter)
	{
		UInventoryComponent* InventoryComp = ThisCharacter->FindComponentByClass<UInventoryComponent>();
		if (IsValid(InventoryComp))
		{
			if (UCollectableComponent* CollectableComp = Cast<UCollectableComponent>(CurrentInteractableComponent))
			{
				// Get name of item from the collectible component and add to inventory
				if (InventoryComp->AddItemToInventory(CollectableComp->GetItemRow()))
				{
					CollectableComp->GetOwner()->Destroy();
					UE_LOG(LogInteraction, Display, TEXT("Successfully added %s to inventory"), *CollectableComp->GetItemRow().RowName.ToString());
				}
				else
				{
					UE_LOG(LogInteraction, Warning, TEXT("Could not add %s to inventory"), *CollectableComp->GetItemRow().RowName.ToString());
				}
			}
		}
		else
		{
			UE_LOG(LogInteraction, Warning, TEXT("No InventoryComponent found on %s"), *ThisCharacter->GetName());
		}
	}
	else
	{
		UE_LOG(LogInteraction, Warning, TEXT("Owner is not a character!"));
	}
	bIsInteracting = false;
}

bool UInteractorComponent::IsChildOfPivotableComponent(UPrimitiveComponent* TargetedComponent, UPivotableComponent* PivotableComp)
{
	if (!IsValid(PivotableComp) || !IsValid(PivotableComp->PivotableParentMeshComponent))
	{
		return false;
	}

	// Direct memory address comparison
	if (TargetedComponent == PivotableComp->PivotableParentMeshComponent)
	{
		return true;
	}

	TArray<USceneComponent*, FDefaultAllocator> Parents;
	TargetedComponent->GetParentComponents(Parents);
	for (USceneComponent* ParentComp : Parents)
	{
		if (ParentComp == PivotableComp->PivotableParentMeshComponent)
		{
			return true;
		}
	}

	return false;
}

void UInteractorComponent::UpdateInteractionPrompt()
{
	// display interaction icon change - need to also consider if we are holding something from the inventory like a key
	UTexture2D* CurrentIcon = nullptr;


	if (CachedActiveItemIcon && bHoldingItem)
	{
		// TODO: Can we make the icon flash if we are in range?
		CurrentIcon = CachedActiveItemIcon;
	}
	else if (!bIsInteracting && ReachableTargetHitResult.bBlockingHit)
	{
		if (UInteractableComponent* HitInteractable = ReachableTargetHitResult.GetActor()->GetComponentByClass<UInteractableComponent>())
		{
			CurrentIcon = HitInteractable->InteractionIcon;
		}
	}
	// BROADCAST: Only fire if the icon pointer actually changed
	if (CurrentIcon != LastInteractionIcon.Get())
	{
		LastInteractionIcon = CurrentIcon;
		// Pass the texture to the UI
		OnInteractionIconChanged.Broadcast(CurrentIcon);
	}
}
