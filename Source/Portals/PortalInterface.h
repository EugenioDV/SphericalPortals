// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "PortalInterface.generated.h"
class ABlackPortal;

// This class does not need to be modified.
UINTERFACE(MinimalAPI, BlueprintType)
class UPortalInterface : public UInterface
{
	GENERATED_BODY()

};

/**
 * 
 */
class PORTALS_API IPortalInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintImplementableEvent, Category = "Teleporting")
		void ReactToTeleportation(const FRotator& DeltaRotation, ABlackPortal* Portal);

};
