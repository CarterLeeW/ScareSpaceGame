// Copyright Carter Wooton

#include "Interaction/RotatableComponent.h"
#include "Interaction/InteractorComponent.h"
#include "Components/SceneComponent.h"

URotatableComponent::URotatableComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void URotatableComponent::BeginPlay()
{
	Super::BeginPlay();

	TArray<UActorComponent*> AllComponents;
	GetOwner()->GetComponents(AllComponents);

	for (UActorComponent* Comp : AllComponents)
	{
		if (Comp->GetFName() == RotatableMeshName)
		{
			RotatableMeshComponent = Cast<USceneComponent>(Comp);
			break;
		}
	}

	if (!RotatableMeshComponent)
	{
		RotatableMeshComponent = GetAttachParent();
	}

	// Store the initial rotation as a Quaternion to act as our mathematical zero-point
	if (RotatableMeshComponent)
	{
		BaseRotationQuat = RotatableMeshComponent->GetRelativeRotation().Quaternion();
	}
}

bool URotatableComponent::BeginInteraction(UInteractorComponent* Interactor)
{
	if (!Super::BeginInteraction(Interactor))
	{
		return false;
	}

	if (!IsValid(RotatableMeshComponent))
	{
		return false;
	}

	bIsBeingHeld = true;
	return true;
}

void URotatableComponent::EndInteraction(UInteractorComponent* Interactor)
{
	Super::EndInteraction(Interactor);
	bIsBeingHeld = false;
}

void URotatableComponent::ProcessInputDelta(FVector2D InputDelta, UInteractorComponent* Interactor)
{
	if (!bIsBeingHeld || !IsValid(RotatableMeshComponent))
	{
		return;
	}

	// 1. Calculate the rotation change (using mouse X-axis for crank turning)
	float DeltaAngle = InputDelta.X * RotationSensitivity;

	// Security: Clamp extreme input spikes to prevent the crank from jumping past limits in a single frame
	DeltaAngle = FMath::Clamp(DeltaAngle, -45.0f, 45.0f);

	// 2. Accumulate the total angle
	CurrentAngle += DeltaAngle;

	// 3. Apply limits if configured
	if (bHasLimits)
	{
		CurrentAngle = FMath::Clamp(CurrentAngle, MinAngle, MaxAngle);
	}

	// 4. Construct the rotational axis vector
	FVector AxisVector = FVector::ZeroVector;
	switch (RotationAxis)
	{
		case ERotationAxis::Pitch:
		{
			AxisVector = FVector(0.0f, 1.0f, 0.0f);
			break;
		}
		case ERotationAxis::Yaw:
		{
			AxisVector = FVector(0.0f, 0.0f, 1.0f);
			break;
		}
		case ERotationAxis::Roll:
		{
			AxisVector = FVector(1.0f, 0.0f, 0.0f);
			break;
		}
	}

	// 5. Build a pure Quaternion representing the exact local offset, and multiply it by the baseline
	FQuat DeltaQuat(AxisVector, FMath::DegreesToRadians(CurrentAngle));
	FQuat NewRotation = BaseRotationQuat * DeltaQuat;

	RotatableMeshComponent->SetRelativeRotation(NewRotation);
}

bool URotatableComponent::IsBoundToMesh(UPrimitiveComponent* HitMesh) const
{
	if (!IsValid(HitMesh))
	{
		return false;
	}

	if (RotatableMeshComponent && HitMesh == RotatableMeshComponent)
	{
		return true;
	}

	return Super::IsBoundToMesh(HitMesh);
}