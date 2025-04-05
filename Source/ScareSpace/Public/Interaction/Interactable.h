// Copyright Carter Wooton

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Interactable.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UInteractable : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class SCARESPACE_API IInteractable
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	/*
	* Should return the Interaction Component associated with the object to be interacted with.
	* This is required for the player to interact with the object.
	*/
	virtual void GetInteractionComponent() = 0;
};
