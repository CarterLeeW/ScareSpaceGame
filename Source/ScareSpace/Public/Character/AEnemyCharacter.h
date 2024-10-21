// Copyright Carter Wooton

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AEnemyCharacter.generated.h"

UCLASS()
class SCARESPACE_API AAEnemyCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AAEnemyCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
