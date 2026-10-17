// Copyright Carter Wooton

#include "Interaction/SimplePivotableComponent.h"
#include "Interaction/InteractorComponent.h"
#include "Components/StaticMeshComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Pawn.h"
#include "Logging/ScareSpaceLogs.h"
#include "DrawDebugHelpers.h"

USimplePivotableComponent::USimplePivotableComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void USimplePivotableComponent::BeginPlay()
{
	Super::BeginPlay();

	TArray<UActorComponent*> AllComponents;
	GetOwner()->GetComponents(AllComponents);

	for (UActorComponent* Comp : AllComponents)
	{
		if (Comp->GetFName() == PivotableParentMeshName)
		{
			PivotableParentMeshComponent = Cast<UStaticMeshComponent>(Comp);
			break;
		}
	}

	if (!PivotableParentMeshComponent)
	{
		PivotableParentMeshComponent = Cast<UStaticMeshComponent>(GetAttachParent());
	}

	if (!PivotableParentMeshComponent)
	{
		UE_LOG(LogInteraction, Error, TEXT("SimplePivotableComponent failed to locate parent mesh component on %s"), *GetOwner()->GetName());

		if (GEngine)
		{
			FString FatalMsg = FString::Printf(TEXT("SimplePivotableComponent requires a valid Mesh Component on %s"), *GetOwner()->GetName());
			GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, FatalMsg);
		}

		PrimaryComponentTick.SetTickFunctionEnable(false);
		return;
	}

	// Disable physics simulation to force kinematic relative movement
	PivotableParentMeshComponent->SetSimulatePhysics(false);

	BaseRelativeRotation = PivotableParentMeshComponent->GetRelativeRotation();
	CurrentPivotAngle = 0.0f;
	TargetPivotAngle = 0.0f;

	UpdateClosedState();
}

void USimplePivotableComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (bIsAnimatingProgrammatically)
	{
		CurrentPivotAngle = FMath::FInterpTo(CurrentPivotAngle, TargetPivotAngle, DeltaTime, ProgrammaticSpeed);
		ApplyRotation(CurrentPivotAngle);

		if (FMath::IsNearlyEqual(CurrentPivotAngle, TargetPivotAngle, 0.1f))
		{
			CurrentPivotAngle = TargetPivotAngle;
			ApplyRotation(CurrentPivotAngle);
			bIsAnimatingProgrammatically = false;
		}

		UpdateClosedState();
	}
}

bool USimplePivotableComponent::BeginInteraction(UInteractorComponent* Interactor)
{
	if (!Super::BeginInteraction(Interactor))
	{
		return false;
	}

	if (bIsLocked)
	{
		UE_LOG(LogInteraction, Log, TEXT("%s is locked - rejecting interaction from %s"), *GetOwner()->GetName(), *Interactor->GetOwner()->GetName());
		return false;
	}

	// Calculate player 's relative position to the pivotable component to determine if we should reverse the drag direction
	if (Interactor && PivotableParentMeshComponent)
	{
		FVector CheckOrigin = PivotableParentMeshComponent->Bounds.Origin;
		FVector PlayerLocation = Interactor->GetOwner()->GetActorLocation();

		// Use Right Vector (+Y) since mesh +X is aligned along the panel length | Negated to get the desired vector
		FVector PivotableFacingVector = bUseMeshForwardVector? PivotableParentMeshComponent->GetForwardVector() : PivotableParentMeshComponent->GetRightVector();
		if (bReverseDirection)
		{
			PivotableFacingVector *= -1.0f;
		}

		FVector DirectionToPlayer = (PlayerLocation - CheckOrigin).GetSafeNormal();

		float DotProduct = FVector::DotProduct(PivotableFacingVector, DirectionToPlayer);
		bIsInteractingFromBehind = (DotProduct < 0.0f);

		// Draw debug
		UWorld* World = GetWorld();
		if (World)
		{
			// Green: Perpendicular Door Face Vector
			DrawDebugDirectionalArrow(World, CheckOrigin, CheckOrigin + (PivotableFacingVector * 100.0f),
				25.0f, FColor::Green, false, 5.0f, 0, 3.0f);

			// Blue: Direction to Player from Mesh Center
			DrawDebugDirectionalArrow(World, CheckOrigin, CheckOrigin + (DirectionToPlayer * 100.0f),
				25.0f, FColor::Blue, false, 5.0f, 0, 3.0f);

			if (GEngine)
			{
				FString DebugMsg = FString::Printf(TEXT("Dot Product: %.3f | Interacting From Behind: %s"),
					DotProduct, bIsInteractingFromBehind ? TEXT("TRUE") : TEXT("FALSE"));

				FColor TextColor = bIsInteractingFromBehind ? FColor::Orange : FColor::Cyan;
				GEngine->AddOnScreenDebugMessage(-1, 5.0f, TextColor, DebugMsg);
			}
		}
	}

	bIsGrabbed = true;
	bIsAnimatingProgrammatically = false;

	AddPivotingMappingContext(Interactor);
	return true;
}

void USimplePivotableComponent::EndInteraction(UInteractorComponent* Interactor)
{
	Super::EndInteraction(Interactor);

	bIsGrabbed = false;
	RemovePivotingMappingContext(Interactor);
	UpdateClosedState();
}

bool USimplePivotableComponent::TryInteractWithItem(const FDataTableRowHandle& CollectableItemRow, UInteractorComponent* Interactor)
{
	if (!InteractableItem.IsNull() && CollectableItemRow == InteractableItem)
	{
		if (bIsLocked)
		{
			bIsLocked = false;
			OnUnlocked();

			if (AActor* Owner = GetOwner())
			{
				TArray<USimplePivotableComponent*> SiblingComponents;
				Owner->GetComponents<USimplePivotableComponent>(SiblingComponents);

				for (USimplePivotableComponent* Sibling : SiblingComponents)
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

void USimplePivotableComponent::OnUnlocked()
{
	OnSimplePivotableLockStateChanged.Broadcast(bIsLocked);
	// Do cosmetic stuff here
}

void USimplePivotableComponent::ProcessInputDelta(FVector2D InputDelta, UInteractorComponent* Interactor)
{
	if (!bIsGrabbed || bIsLocked || !PivotableParentMeshComponent)
	{
		return;
	}

	float RawDelta = InputDelta.Y;
	if (bInvertDragAxis)
	{
		RawDelta *= -1.0f;
	}
	
	if (bIsInteractingFromBehind)
	{
		RawDelta *= -1.0f;
	}

	float AngleChange = RawDelta * DragSensitivity;
	CurrentPivotAngle = FMath::Clamp(CurrentPivotAngle + AngleChange, 0.0f, MaxOpenAngle);

	ApplyRotation(CurrentPivotAngle);
	UpdateClosedState();
}

void USimplePivotableComponent::OpenPivotableProgrammatically(float TargetAngle, float Speed, bool bBypassLock)
{
	if (bIsLocked && !bBypassLock)
	{
		return;
	}

	bIsGrabbed = false;
	bIsAnimatingProgrammatically = true;
	TargetPivotAngle = FMath::Clamp(TargetAngle, 0.0f, MaxOpenAngle);
	ProgrammaticSpeed = Speed;
}

void USimplePivotableComponent::ClosePivotableProgrammatically(float Speed)
{
	OpenPivotableProgrammatically(0.0f, Speed);
}

void USimplePivotableComponent::SetIsLocked(bool bInLocked)
{
	if (bIsLocked != bInLocked)
	{
		bIsLocked = bInLocked;
		OnSimplePivotableLockStateChanged.Broadcast(bIsLocked);
	}
}

void USimplePivotableComponent::ApplyRotation(float NewAngle)
{
	if (!PivotableParentMeshComponent)
	{
		return;
	}

	float DirectionMultiplier = bReverseDirection ? -1.0f : 1.0f;

	FRotator NewRelativeRotation = BaseRelativeRotation;
	NewRelativeRotation.Yaw += NewAngle * DirectionMultiplier;

	PivotableParentMeshComponent->SetRelativeRotation(NewRelativeRotation);
}

void USimplePivotableComponent::UpdateClosedState()
{
	bool bNewAtRest = (FMath::Abs(CurrentPivotAngle) <= ClosedAngleThreshold);

	if (bNewAtRest != bIsAtRest)
	{
		bIsAtRest = bNewAtRest;
		OnSimplePivotableRestStateChanged.Broadcast(bIsAtRest);
	}
}

void USimplePivotableComponent::AddPivotingMappingContext(UInteractorComponent* Interactor)
{
	if (!PivotingMappingContext || !Interactor)
	{
		return;
	}

	if (APawn* InteractorPawn = Cast<APawn>(Interactor->GetOwner()))
	{
		if (APlayerController* PC = Cast<APlayerController>(InteractorPawn->GetController()))
		{
			if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
			{
				Subsystem->AddMappingContext(PivotingMappingContext, MappingContextPriority);
			}
		}
	}
}

void USimplePivotableComponent::RemovePivotingMappingContext(UInteractorComponent* Interactor)
{
	if (!PivotingMappingContext || !Interactor)
	{
		return;
	}

	if (APawn* InteractorPawn = Cast<APawn>(Interactor->GetOwner()))
	{
		if (APlayerController* PC = Cast<APlayerController>(InteractorPawn->GetController()))
		{
			if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
			{
				Subsystem->RemoveMappingContext(PivotingMappingContext);
			}
		}
	}
}