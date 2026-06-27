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

bool UPivotableComponent::TryInteractWithItem(FDataTableRowHandle ItemRow)
{
	// Default behavior: check if the item matches the requirement
	if (!InteractableItem.IsNull() && ItemRow == InteractableItem)
	{
		if (bIsLocked && bCanClose)
		{
			bIsLocked = false;
			// Handle unlocking cosmetics
		}
		return true;
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
