// Copyright Carter Wooton

#include "Interaction/PivotableComponent.h"
#include "Interaction/InteractorComponent.h"
#include "PhysicsEngine/PhysicsConstraintComponent.h"
#include "Components/StaticMeshComponent.h"
#include "PhysicsEngine/PhysicsHandleComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/PlayerController.h"
#include "Logging/ScareSpaceLogs.h"

UPivotableComponent::UPivotableComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = true;
}

void UPivotableComponent::BeginPlay()
{
	Super::BeginPlay();

	TArray<UActorComponent*> AllComponents;
	GetOwner()->GetComponents(AllComponents);

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

	if (!PivotableParentMeshComponent)
	{
		PivotableParentMeshComponent = Cast<UStaticMeshComponent>(GetAttachParent());
	}

	if (PivotableParentMeshComponent && HingeComponent)
	{
		BaseRotation = PivotableParentMeshComponent->GetComponentRotation();
		HingeComponent->SetRelativeRotation(HingeStartingRotation);
		PivotableParentMeshComponent->SetSimulatePhysics(true);
	}

	if (!bCanClose && PhysicsConstraintComponent)
	{
		PhysicsConstraintComponent->SetAngularDriveParams(0.0f, 0.0f, 0.0f);
		bIsClosed = false;
		bIsLocked = false;
	}
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

bool UPivotableComponent::BeginInteraction(UInteractorComponent* Interactor)
{
	if (!Super::BeginInteraction(Interactor))
	{
		return false;
	}

	if (bIsLocked)
	{
		return false;
	}

	UPrimitiveComponent* ComponentToHold = PivotableParentMeshComponent ? PivotableParentMeshComponent.Get() : Cast<UPrimitiveComponent>(GetAttachParent());
	UPhysicsHandleComponent* PhysicsHandle = Interactor->GetPhysicsHandle();

	if (!IsValid(ComponentToHold) || !IsValid(PhysicsHandle))
	{
		return false;
	}

	bIsBeingHeld = true;
	ComponentToHold->SetSimulatePhysics(true);
	ComponentToHold->WakeAllRigidBodies();

	TargetHoldLength = FVector::Dist(Interactor->GetComponentLocation(), Interactor->GetReachableTargetHitResult().ImpactPoint);
	TargetSideLength = 0.0f;

	FVector TargetLocation = Interactor->GetComponentLocation() + (Interactor->GetForwardVector() * TargetHoldLength);
	PhysicsHandle->SetTargetLocationAndRotation(TargetLocation, Interactor->GetComponentRotation());
	PhysicsHandle->GrabComponentAtLocationWithRotation(
		ComponentToHold,
		NAME_None,
		Interactor->GetReachableTargetHitResult().ImpactPoint,
		Interactor->GetComponentRotation()
	);

	if (APlayerController* PC = Interactor->GetPlayerController())
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
		{
			if (Interactor->PivotingMappingContext)
			{
				Subsystem->AddMappingContext(Interactor->PivotingMappingContext, 5);
			}
		}
	}

	return true;
}

void UPivotableComponent::ContinueInteraction(UInteractorComponent* Interactor)
{
	if (!bIsBeingHeld || !IsValid(Interactor))
	{
		return;
	}

	float CurrentHeldLength = FVector::Dist(Interactor->GetComponentLocation(), GetOwner()->GetActorLocation());
	if (CurrentHeldLength > Interactor->HoldAutoDropDistance)
	{
		EndInteraction(Interactor);
		return;
	}

	UPhysicsHandleComponent* PhysicsHandle = Interactor->GetPhysicsHandle();
	if (IsValid(PhysicsHandle))
	{
		FVector StartLoc = Interactor->GetComponentLocation();
		FVector ForwardLoc = Interactor->GetForwardVector() * TargetHoldLength;
		FVector SideLoc = Interactor->GetOwner()->GetActorRightVector() * TargetSideLength;
		FVector TargetLocation = StartLoc + ForwardLoc + SideLoc;

		PhysicsHandle->SetTargetLocationAndRotation(TargetLocation, Interactor->GetComponentRotation());
	}
}

void UPivotableComponent::ProcessInputDelta(FVector2D InputDelta, UInteractorComponent* Interactor)
{
	if (!bIsBeingHeld)
	{
		return;
	}

	float PushPullSensitivity = 2.0f;
	float LateralSensitivity = 2.0f;

	TargetHoldLength = FMath::Clamp(TargetHoldLength + (InputDelta.Y * PushPullSensitivity), 50.0f, 300.0f);
	TargetSideLength = FMath::Clamp(TargetSideLength + (InputDelta.X * LateralSensitivity), -200.0f, 200.0f);
}

void UPivotableComponent::EndInteraction(UInteractorComponent* Interactor)
{
	Super::EndInteraction(Interactor);

	bIsBeingHeld = false;

	if (IsValid(Interactor))
	{
		UPhysicsHandleComponent* PhysicsHandle = Interactor->GetPhysicsHandle();
		if (IsValid(PhysicsHandle) && PhysicsHandle->GetGrabbedComponent())
		{
			PhysicsHandle->ReleaseComponent();
		}
	}
}

bool UPivotableComponent::TryInteractWithItem(const FDataTableRowHandle& CollectableItemRow, UInteractorComponent* Interactor)
{
	if (!InteractableItem.IsNull() && CollectableItemRow == InteractableItem)
	{
		if (bIsLocked)
		{
			bIsLocked = false;
			OnUnlocked();

			if (AActor* Owner = GetOwner())
			{
				TArray<UPivotableComponent*> SiblingComponents;
				Owner->GetComponents<UPivotableComponent>(SiblingComponents);

				for (UPivotableComponent* Sibling : SiblingComponents)
				{
					if (Sibling && (Sibling != this) && Sibling->bIsLocked && (Sibling->InteractableItem == InteractableItem))
					{
						Sibling->bIsLocked = false;
					}
				}
			}
		}
		OnInteractWithItem.Broadcast(Interactor);
		return true;
	}
	return false;
}

bool UPivotableComponent::IsBoundToMesh(UPrimitiveComponent* HitMesh) const
{
	if (!IsValid(HitMesh))
	{
		return false;
	}

	if (PivotableParentMeshComponent && HitMesh == PivotableParentMeshComponent)
	{
		return true;
	}

	return Super::IsBoundToMesh(HitMesh);
}

void UPivotableComponent::OnThrow(UInteractorComponent* Interactor)
{
	if (!IsValid(Interactor)) return;

	UPhysicsHandleComponent* PhysicsHandle = Interactor->GetPhysicsHandle();
	if (!IsValid(PhysicsHandle) || !PhysicsHandle->GetGrabbedComponent()) return;

	UPrimitiveComponent* ComponentToThrow = PhysicsHandle->GetGrabbedComponent();
	if (!IsValid(ComponentToThrow)) return;

	// Wake the physics body just in case it went to sleep while holding
	ComponentToThrow->WakeAllRigidBodies();

	float ObjectMass = ComponentToThrow->GetMass();
	float PivotThrowMultiplier = 0.5f;

	// Calculate the raw impulse force
	FVector Forward = Interactor->GetForwardVector();
	FVector ThrowImpulse = Forward * TargetHoldLength * Interactor->ThrowForceMultiplier * PivotThrowMultiplier * ObjectMass;

	// Find exactly where the player is currently holding the door
	FVector GrabLocation = Interactor->GetComponentLocation() + (Forward * TargetHoldLength);

	// Apply the force at the grab location to generate proper leverage/torque around the hinge
	ComponentToThrow->AddImpulseAtLocation(ThrowImpulse, GrabLocation, NAME_None);

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
	UE_LOG(LogInteraction, Display, TEXT("Unlocking cosmetics executed."));
}