// Copyright Carter Wooton

#include "Interaction/HoldableComponent.h"
#include "Interaction/InteractorComponent.h"
#include "Components/StaticMeshComponent.h"
#include "PhysicsEngine/PhysicsHandleComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/PlayerController.h"
#include "Logging/ScareSpaceLogs.h"

UHoldableComponent::UHoldableComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UHoldableComponent::BeginPlay()
{
	Super::BeginPlay();

	if (UStaticMeshComponent* Mesh = GetOwner()->FindComponentByClass<UStaticMeshComponent>())
	{
		Mesh->OnComponentHit.AddDynamic(this, &UHoldableComponent::OnMeshComponentHit);
	}
}

bool UHoldableComponent::BeginInteraction(UInteractorComponent* Interactor)
{
	if (!Super::BeginInteraction(Interactor))
	{
		return false;
	}

	UPrimitiveComponent* ComponentToHold = Interactor->GetReachableTargetHitResult().GetComponent();
	UPhysicsHandleComponent* PhysicsHandle = Interactor->GetPhysicsHandle();

	if (!IsValid(ComponentToHold) || !IsValid(PhysicsHandle))
	{
		return false;
	}

	bIsBeingHeld = true;
	ComponentToHold->SetSimulatePhysics(true);
	ComponentToHold->WakeAllRigidBodies();

	if (UStaticMeshComponent* Mesh = Cast<UStaticMeshComponent>(ComponentToHold))
	{
		Mesh->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	}

	TargetHoldLength = FVector::Dist(Interactor->GetComponentLocation(), Interactor->GetReachableTargetHitResult().ImpactPoint);
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
			if (Interactor->HoldingObjectMappingContext)
			{
				Subsystem->AddMappingContext(Interactor->HoldingObjectMappingContext, 5);
			}
		}
	}

	return true;
}

void UHoldableComponent::ContinueInteraction(UInteractorComponent* Interactor)
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
		FVector TargetLocation = Interactor->GetComponentLocation() + (Interactor->GetForwardVector() * TargetHoldLength);
		PhysicsHandle->SetTargetLocationAndRotation(TargetLocation, Interactor->GetComponentRotation());
	}
}

void UHoldableComponent::EndInteraction(UInteractorComponent* Interactor)
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

	if (UStaticMeshComponent* Mesh = GetOwner()->FindComponentByClass<UStaticMeshComponent>())
	{
		Mesh->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);
	}
}

void UHoldableComponent::OnThrow(UInteractorComponent* Interactor)
{
	if (!IsValid(Interactor))
	{
		return;
	}

	UPhysicsHandleComponent* PhysicsHandle = Interactor->GetPhysicsHandle();
	if (!IsValid(PhysicsHandle) || !PhysicsHandle->GetGrabbedComponent())
	{
		return;
	}

	UPrimitiveComponent* ComponentToThrow = PhysicsHandle->GetGrabbedComponent();
	if (!IsValid(ComponentToThrow))
	{
		return;
	}

	float ObjectMass = ComponentToThrow->GetMass();
	ComponentToThrow->SetSimulatePhysics(true);
	ComponentToThrow->WakeAllRigidBodies();

	FVector Forward = Interactor->GetForwardVector();
	FVector ThrowDirection = (Forward + FVector::UpVector * 0.2f).GetSafeNormal();
	FVector ThrowVelocity = (ThrowDirection * TargetHoldLength * Interactor->ThrowForceMultiplier) / ObjectMass;

	ComponentToThrow->AddImpulse(ThrowVelocity, NAME_None, true);

	PhysicsHandle->ReleaseComponent();
	EndInteraction(Interactor);
}

void UHoldableComponent::OnMeshComponentHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (bIsBreakable && (NormalImpulse.Length() > BreakForce))
	{
		// Chaos physics breakage interface call goes here
	}
}