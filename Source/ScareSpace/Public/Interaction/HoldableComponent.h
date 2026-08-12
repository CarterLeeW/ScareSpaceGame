// Copyright Carter Wooton

#pragma once

#include "CoreMinimal.h"
#include "Interaction/InteractableComponent.h"
#include "HoldableComponent.generated.h"

UCLASS(ClassGroup = (Interaction), meta = (BlueprintSpawnableComponent))
class SCARESPACE_API UHoldableComponent : public UInteractableComponent
{
	GENERATED_BODY()

public:
	UHoldableComponent();

	virtual bool BeginInteraction(UInteractorComponent* Interactor) override;
	virtual void ContinueInteraction(UInteractorComponent* Interactor) override;
	virtual void EndInteraction(UInteractorComponent* Interactor) override;
	virtual void OnThrow(UInteractorComponent* Interactor) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
	bool bIsBreakable = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
	float BreakForce = 1000.0f;

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnMeshComponentHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

private:
	bool bIsBeingHeld = false;
	float TargetHoldLength = 0.0f;
};