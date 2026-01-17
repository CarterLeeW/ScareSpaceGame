// Copyright Carter Wooton


#include "Interaction/PivotableComponent.h"
#include "Logging/ScareSpaceLogs.h"
#include "PhysicsEngine/PhysicsConstraintComponent.h"


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
        // locked cosmetics
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
    // maybe we don't even need to tick if it can't close
    if (bCanClose)
    {
		UpdateClosedState();
        if (bIsClosed && !bIsBeingHeld)
        {
            // need to check the close target angle
            PhysicsConstraintComponent->SetAngularDriveParams(ClosedStrength, 0.0f, 0.0f);
        }
        else
        {
			PhysicsConstraintComponent->SetAngularDriveParams(0.0f, 0.0f, 0.0f);
        }
    }
}

void UPivotableComponent::BeginPlay()
{
	Super::BeginPlay();
	// Find the pivotable parent mesh component somewhere on this actor by name
    PivotableParentMeshComponent = Cast<UStaticMeshComponent>(GetOwner()->GetDefaultSubobjectByName(PivotableParentMeshName));
    if (!PivotableParentMeshComponent)
    {
        UE_LOG(LogInteraction, Warning, TEXT("PivotableComponent: Could not find PivotableParentMeshComponent with name %s on actor %s"), *PivotableParentMeshName.ToString(), *GetOwner()->GetName());
	}
    // do same for hinge component
	HingeComponent = Cast<USceneComponent>(GetOwner()->GetDefaultSubobjectByName(HingeComponentName));
    if (!HingeComponent)
    {
        UE_LOG(LogInteraction, Warning, TEXT("PivotableComponent: Could not find HingeComponent with name %s on actor %s"), *HingeComponentName.ToString(), *GetOwner()->GetName());
	}
	PhysicsConstraintComponent = GetOwner()->GetComponentByClass<UPhysicsConstraintComponent>();
    if (!PhysicsConstraintComponent)
    {
        UE_LOG(LogInteraction, Warning, TEXT("PivotableComponent: Could not find PhysicsConstraintComponent on actor %s"), *GetOwner()->GetName());
	}

	// Perform necessary default setup
	BaseRotation = PivotableParentMeshComponent->GetComponentRotation();
	HingeComponent->SetRelativeRotation(HingeStartingRotation);
	PivotableParentMeshComponent->SetSimulatePhysics(true);
}

void UPivotableComponent::UpdateClosedState()
{
	FRotator DeltaRotator = PivotableParentMeshComponent->GetComponentRotation() - BaseRotation;
    DeltaRotator.Normalize();
	float YawAngle = FMath::Abs(DeltaRotator.Yaw);
    if (YawAngle <= ClosedAngle)
    {
        bIsClosed = true;
    }
    else
    {
        bIsClosed = false;
	}
}
