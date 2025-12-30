// Copyright Carter Wooton


#include "Interaction/PivotableComponent.h"
#include "Components/PrimitiveComponent.h"
#include "PhysicsEngine/PhysicsConstraintComponent.h"

UPivotableComponent::UPivotableComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
    PrimaryComponentTick.bStartWithTickEnabled = false;
	InteractableType = EInteractableType::Pivotable;
}

void UPivotableComponent::BeginInteraction()
{
    bIsBeingHeld = true;
}

void UPivotableComponent::EndInteraction()
{
    bIsBeingHeld = false;
}

void UPivotableComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
    if (!PivotableMesh || !ConstraintComponent) return;

    if (bEnableLatching && !bIsLinearMovement)
    {
        // Get current angle (Assuming Yaw/Z rotation relative to parent)
        float CurrentYaw = PivotableMesh->GetRelativeRotation().Yaw;

        bool bNearMin = FMath::IsNearlyEqual(CurrentYaw, MinLimit, LatchAngleThreshold);
        bool bNearMax = FMath::IsNearlyEqual(CurrentYaw, MaxLimit, LatchAngleThreshold);

        if (bNearMin)
        {
            // Snap to Closed (Min)
            ConstraintComponent->SetAngularOrientationTarget(FRotator(0, MinLimit, 0));
            ConstraintComponent->SetAngularOrientationDrive(true, true);
            ConstraintComponent->SetAngularDriveParams(LatchStrength, 100.0f, 0.0f);
        }
        else if (bNearMax)
        {
            // Snap to Open (Max)
            ConstraintComponent->SetAngularOrientationTarget(FRotator(0, MaxLimit, 0));
            ConstraintComponent->SetAngularOrientationDrive(true, true);
            ConstraintComponent->SetAngularDriveParams(LatchStrength, 100.0f, 0.0f);
        }
        else
        {
            // Free Swing (Disable Motor)
            ConstraintComponent->SetAngularOrientationDrive(false, false);
        }

        // stay awake
        if (bIsBeingHeld) return;

        // Condition B: If the door has stopped moving, go back to sleep.
        if (PivotableMesh->GetPhysicsAngularVelocityInDegrees().IsNearlyZero(1.0f))
        {
            SetComponentTickEnabled(false);
        }
    }
}

void UPivotableComponent::BeginPlay()
{
	Super::BeginPlay();
    if (!PivotableParentMeshName.IsNone())
    {
        TArray<UPrimitiveComponent*> AllComps;
        GetOwner()->GetComponents(AllComps);

        for (UPrimitiveComponent* Comp : AllComps)
        {
            // check if the component's name matches our variable
            if (Comp->GetFName() == PivotableParentMeshName)
            {
                PivotableMesh = Comp;
                break; // Found it, stop searching
            }
        }
    }

    if (PivotableMesh)
    {
        // Ensure it generates wake events (required for the optimization to work)
        PivotableMesh->BodyInstance.bGenerateWakeEvents = true;

        // Bind the event
        PivotableMesh->OnComponentWake.AddDynamic(this, &UPivotableComponent::OnMeshWake);
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("%s: Could not find a mesh named '%s'! Check your spelling."),
            *GetOwner()->GetName(),
            *PivotableParentMeshName.ToString());

        // Disable everything to prevent crashes
        SetComponentTickEnabled(false);
        return;
    }

    ConstraintComponent = GetOwner()->FindComponentByClass<UPhysicsConstraintComponent>();
    if (!ConstraintComponent)
    {
        UE_LOG(LogTemp, Warning, TEXT("%s: PivotableComponent needs a PhysicsConstraintComponent to work!"), *GetOwner()->GetName());
    }
    // Start Asleep
    SetComponentTickEnabled(false);
}

void UPivotableComponent::OnMeshWake(UPrimitiveComponent* WakingComponent, FName BoneName)
{
	SetComponentTickEnabled(true);
}
