// Copyright Carter Wooton


#include "Interaction/InteractorComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/PlayerController.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"

// Sets default values for this component's properties
UInteractorComponent::UInteractorComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

}


// Called when the game starts
void UInteractorComponent::BeginPlay()
{
	Super::BeginPlay();
	ACharacter* ThisChar = Cast<ACharacter>(GetOwner());
	APlayerController* ThisController = Cast<APlayerController>(ThisChar->GetController());
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
		ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(ThisController->GetLocalPlayer()))
	{
		Subsystem->AddMappingContext(InteractorMappingContext, 0);
	}
}


// Called every frame
void UInteractorComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

