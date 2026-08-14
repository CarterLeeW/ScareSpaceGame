// Copyright Carter Wooton

#include "Interaction/RotatableComponent.h"
#include "Interaction/InteractorComponent.h"
#include "Components/SceneComponent.h"
#include "EnhancedInputSubsystems.h"

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

	VirtualMousePosition = FVector2D(0.0f, -100.0f);
	PreviousMouseAngle = FMath::Atan2(VirtualMousePosition.Y, VirtualMousePosition.X);

	if (APlayerController* PC = Interactor->GetPlayerController())
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
		{
			// Reusing PivotingMappingContext since it already contains your mouse X/Y delta bindings
			if (Interactor->PivotingMappingContext)
			{
				Subsystem->AddMappingContext(Interactor->PivotingMappingContext, 5);
			}
		}
	}

	return true;
}

void URotatableComponent::ContinueInteraction(UInteractorComponent* Interactor)
{
	if (!bIsBeingHeld || !IsValid(Interactor))
	{
		return;
	}

	float CurrentHeldLength = FVector::Dist(Interactor->GetComponentLocation(), GetOwner()->GetActorLocation());
	if (CurrentHeldLength > Interactor->HoldAutoDropDistance)
	{
		Interactor->RequestEndInteraction();
	}
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

	// Calculate pure visual mouse movement
	VirtualMousePosition.X += InputDelta.X * RotationSensitivity;
	VirtualMousePosition.Y += InputDelta.Y * RotationSensitivity;
	VirtualMousePosition = VirtualMousePosition.GetSafeNormal() * 100.0f;

	float CurrentMouseAngle = FMath::Atan2(VirtualMousePosition.Y, VirtualMousePosition.X);
	float DeltaAngleRad = CurrentMouseAngle - PreviousMouseAngle;
	DeltaAngleRad = FMath::Atan2(FMath::Sin(DeltaAngleRad), FMath::Cos(DeltaAngleRad));

	// A clockwise mouse movement on screen results in a positive VisualDelta
	float VisualDelta = FMath::RadiansToDegrees(DeltaAngleRad);
	PreviousMouseAngle = CurrentMouseAngle;

	// Identify the local axis of the mesh
	FVector LocalAxisVector = FVector::ZeroVector;
	switch (RotationAxis)
	{
	case ERotationAxis::Pitch: LocalAxisVector = FVector(0.0f, 1.0f, 0.0f); break;
	case ERotationAxis::Yaw:   LocalAxisVector = FVector(0.0f, 0.0f, 1.0f); break;
	case ERotationAxis::Roll:  LocalAxisVector = FVector(1.0f, 0.0f, 0.0f); break;
	}

	// Determine perspective multipliers
	FVector WorldAxisVector = RotatableMeshComponent->GetComponentTransform().TransformVectorNoScale(LocalAxisVector);
	FVector PlayerForward = Interactor->GetForwardVector();

	float PerspectiveMultiplier = FVector::DotProduct(PlayerForward, WorldAxisVector) < 0.0f ? -1.0f : 1.0f;
	float InvertMultiplier = bInvertRotation ? -1.0f : 1.0f;

	// Update the rotation progress based on perspective and inversion settings
	CurrentAngle += (VisualDelta * PerspectiveMultiplier * InvertMultiplier);

	if (bHasLimits)
	{
		CurrentAngle = FMath::Clamp(CurrentAngle, MinAngle, MaxAngle);
		OnRotated.Broadcast(CurrentAngle);
	}

	// Apply the rotation to the physical mesh. 
	// Re-apply the InvertMultiplier so the visual spin matches the mouse gesture, 
	// while keeping CurrentAngle safely within positive limits.
	float AppliedMeshAngle = CurrentAngle * InvertMultiplier;

	FQuat DeltaQuat(LocalAxisVector, FMath::DegreesToRadians(AppliedMeshAngle));
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