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
UCLASS()
class SCARESPACE_API USimpleFlashlightComponent : public USpotLightComponent
{
	GENERATED_BODY()

public:
	USimpleFlashlightComponent();

	UFUNCTION(BlueprintCallable, Category = "Flashlight")
	void ToggleFlashlight();
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

private:
	bool bIsOn = false;
	bool bCanUse = true;
	
};
