// Copyright Carter Wooton

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "PlayerCharacter.generated.h"

class UInputComponent;
class USkeletalMeshComponent;
class UCameraComponent;
class UInputAction;
class UInputMappingContext;
class UInteractorComponent;
class UPhysicsHandleComponent;
struct FInputActionValue;
class UInventoryComponent;
class UJournalComponent;



UCLASS()
class SCARESPACE_API APlayerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	APlayerCharacter();

	/** Returns Mesh1P subobject **/
	USkeletalMeshComponent* GetMesh1P() const { return Mesh1P; }
	/** Returns FirstPersonCameraComponent subobject **/
	UCameraComponent* GetFirstPersonCameraComponent() const { return FirstPersonCameraComponent; }
	// Returns InteractorComponent
	UFUNCTION(BlueprintCallable, Category = "Interaction")
	UInteractorComponent* GetInteractorComponent() const { return InteractorComponent; }
	// Returns PhysicsHandleComponent
	UPhysicsHandleComponent* GetPhysicsHandleComponent() const { return PhysicsHandleComponent; }
	// Returns InventoryComponent
	UInventoryComponent* GetInventoryComponent() const { return InventoryComponent; }
	// Returns JournalComponent
	UJournalComponent* GetJournalComponent() const { return JournalComponent; }

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	/** Pawn mesh: 1st person view (arms; seen only by self) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Mesh)
	TObjectPtr<USkeletalMeshComponent> Mesh1P;

	/** First person camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	TObjectPtr<UCameraComponent> FirstPersonCameraComponent;

	// Interactor Component
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Interaction)
	TObjectPtr<UInteractorComponent> InteractorComponent;

	// Physics Hanlde
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Physics")
	TObjectPtr<UPhysicsHandleComponent> PhysicsHandleComponent;

	// Inventory Component
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory")
	TObjectPtr<UInventoryComponent> InventoryComponent;

	// Journal Component
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Journal")
	TObjectPtr<UJournalComponent> JournalComponent;

};
