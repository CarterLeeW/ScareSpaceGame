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

void UPivotableComponent::BeginPlay()
{
	Super::BeginPlay();

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
		BaseRotation = PivotableParentMeshComponent->GetRelativeRotation();
		HingeComponent->SetRelativeRotation(HingeStartingRotation);
		PivotableParentMeshComponent->SetSimulatePhysics(true);
	}
}

void UPivotableComponent::UpdateClosedState()
{
	if (!PivotableParentMeshComponent)
	{
		return;
	}

	FRotator DeltaRotator = PivotableParentMeshComponent->GetRelativeRotation() - BaseRotation;
	DeltaRotator.Normalize();

	float YawAngle = FMath::Abs(DeltaRotator.Yaw);
	bIsClosed = (YawAngle <= ClosedAngle);
}
