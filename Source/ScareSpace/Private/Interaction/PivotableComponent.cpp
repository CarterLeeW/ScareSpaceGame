// Copyright Carter Wooton

#include "Interaction/PivotableComponent.h"
#include "Logging/ScareSpaceLogs.h"
#include "PhysicsEngine/PhysicsConstraintComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SceneComponent.h"

UPivotableComponent::UPivotableComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = true;
	InteractableType = EInteractableType::Pivotable;
}

void UPivotableComponent::BeginInteraction()
{
	InteractionCounter = FMath::Clamp(InteractionCounter + 1, 0, 255);
	OnInteractionBegins.Broadcast();
	if (bIsLocked)
	{
		// Handle locked cosmetics
	}
	else
	{
		bIsBeingHeld = true;
	}
}

void UPivotableComponent::EndInteraction()
{
	OnInteractionEnds.Broadcast();
	bIsBeingHeld = false;
}

void UPivotableComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (bCanClose)
	{
		UpdateClosedState();

		if (PhysicsConstraintComponent)
		{
			if (bIsClosed && !bIsBeingHeld)
			{
				PhysicsConstraintComponent->SetAngularDriveParams(ClosedStrength, 0.0f, 0.0f);
			}
			else
			{
				PhysicsConstraintComponent->SetAngularDriveParams(0.0f, 0.0f, 0.0f);
			}
		}
	}
}

bool UPivotableComponent::TryInteractWithItem(FDataTableRowHandle CollectableItemRow)
{
	// Default behavior: check if the item matches the requirement
	if (!InteractableItem.IsNull() && CollectableItemRow == InteractableItem)
	{
		if (bIsLocked)
		{
			bIsLocked = false;
			// Handle unlocking cosmetics
			OnUnlocked();

			// Chain-unlock matching sibling doors or other pivotables on same Actor
			if (AActor* Owner = GetOwner())
			{
				TArray<UPivotableComponent*> SiblingComponents;
				Owner->GetComponents<UPivotableComponent>(SiblingComponents);

				for (UPivotableComponent* Sibling : SiblingComponents)
				{
					if (Sibling && (Sibling != this) && Sibling->bIsLocked && (Sibling->InteractableItem == InteractableItem))
					{
						Sibling->bIsLocked = false;
						// Only call one OnUnlocked() to avoid multiple cosmetic triggers if needed
						// Sibling->OnUnlocked();
					}
				}
			}
		}
		OnInteractWithItem.Broadcast();
		return true; // Authorizes consuming the key
	}
	return false;
}

void UPivotableComponent::BeginPlay()
{
	Super::BeginPlay();

	// Retrieve all components attached to the owning actor
	TArray<UActorComponent*> AllComponents;
	GetOwner()->GetComponents(AllComponents);

	// Iterate through and match the FNames
	for (UActorComponent* Comp : AllComponents)
	{
		if (Comp->GetFName() == PivotableParentMeshName)
		{
			PivotableParentMeshComponent = Cast<UStaticMeshComponent>(Comp);
		}
		else if (Comp->GetFName() == HingeComponentName)
		{
			HingeComponent = Cast<USceneComponent>(Comp);
		}
		else if (Comp->GetFName() == PhysicsConstraintName)
		{
			PhysicsConstraintComponent = Cast<UPhysicsConstraintComponent>(Comp);
		}
	}

	// Validate pointers assigned in the editor to prevent dereferencing null
	if (!PivotableParentMeshComponent)
	{
		UE_LOG(LogInteraction, Error, TEXT("PivotableComponent: PivotableParentMeshComponent is not assigned on actor %s"), *GetOwner()->GetName());
	}
	if (!HingeComponent)
	{
		UE_LOG(LogInteraction, Error, TEXT("PivotableComponent: HingeComponent is not assigned on actor %s"), *GetOwner()->GetName());
	}
	if (!PhysicsConstraintComponent)
	{
		UE_LOG(LogInteraction, Error, TEXT("PivotableComponent: PhysicsConstraintComponent is not assigned on actor %s"), *GetOwner()->GetName());
	}

	if (PivotableParentMeshComponent && HingeComponent)
	{
		BaseRotation = PivotableParentMeshComponent->GetComponentRotation();
		HingeComponent->SetRelativeRotation(HingeStartingRotation);
		PivotableParentMeshComponent->SetSimulatePhysics(true);
	}
	// Prevents locking a door that shouldn't close and ensures the door can always swing
	if (!bCanClose && PhysicsConstraintComponent)
	{
		PhysicsConstraintComponent->SetAngularDriveParams(0.0f, 0.0f, 0.0f);
		bIsClosed = false;
		bIsLocked = false;
	}
}

void UPivotableComponent::UpdateClosedState()
{
	if (!PivotableParentMeshComponent)
	{
		return;
	}

	FRotator DeltaRotator = PivotableParentMeshComponent->GetComponentRotation() - BaseRotation;
	DeltaRotator.Normalize();

	float YawAngle = FMath::Abs(DeltaRotator.Yaw);
	bIsClosed = (YawAngle <= ClosedAngle);
}

void UPivotableComponent::OnUnlocked()
{
	UE_LOG(LogInteraction, Display, TEXT("Unlocking cosmetics"));
}
