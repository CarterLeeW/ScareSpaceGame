// Copyright Carter Wooton

#pragma once

#include "CoreMinimal.h"
#include "Components/SpotLightComponent.h"
#include "SimpleFlashlightComponent.generated.h"

class UInputMappingContext;
class UInputAction;

/**
 * 
 */
UCLASS(ClassGroup = (Gameplay), meta = (BlueprintSpawnableComponent))
class SCARESPACE_API USimpleFlashlightComponent : public USpotLightComponent
{
	GENERATED_BODY()

public:
	USimpleFlashlightComponent();

	// Toggles flashlight on or off if allowed
	UFUNCTION(BlueprintCallable, Category = "Flashlight")
	void ToggleFlashlight();
	// Forces flashlight on
	UFUNCTION(BlueprintCallable, Category = "Flashlight")
	void TurnFlashlightOn();
	// Forces flashlight off
	UFUNCTION(BlueprintCallable, Category = "Flashlight")
	void TurnFlashlightOff();
	UFUNCTION(BlueprintCallable, Category = "Flashlight")
	void SetCanUse(bool bNewCanUse) { bCanUse = bNewCanUse; }
	UFUNCTION(BlueprintCallable, Category = "Flashlight")
	bool GetIsOn() const { return bIsOn; }
	UFUNCTION(BlueprintCallable, Category = "Flashlight")
	bool GetCanUse() const { return bCanUse; }

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flashlight")
	float BaseIntensity = 5000.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Flashlight")
	UInputMappingContext* FlashlightMappingContext;
	UPROPERTY(EditDefaultsOnly, Category = "Flashlight")
	UInputAction* ToggleFlashlightAction;
	

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio", meta = (AllowPrivateAccess = "true"))
	USoundBase* FlashlightOnSound;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio", meta = (AllowPrivateAccess = "true"))
	USoundBase* FlashlightOffSound;

private:
	bool bIsOn = false;
	bool bCanUse = true;
	
};
