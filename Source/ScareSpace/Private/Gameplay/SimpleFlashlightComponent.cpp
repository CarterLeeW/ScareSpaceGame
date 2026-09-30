// Copyright Carter Wooton


#include "Gameplay/SimpleFlashlightComponent.h"

USimpleFlashlightComponent::USimpleFlashlightComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	Intensity = BaseIntensity;
}

void USimpleFlashlightComponent::ToggleFlashlight()
{

}

void USimpleFlashlightComponent::BeginPlay()
{
}
