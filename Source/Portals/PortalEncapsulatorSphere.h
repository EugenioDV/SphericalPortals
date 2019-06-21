// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SphereComponent.h"
#include "PortalEncapsulatorSphere.generated.h"

/**
 * 
 */
UCLASS(ClassGroup = "Portal", hidecategories = (Object, LOD, Lighting, TextureStreaming), editinlinenew, meta = (DisplayName = "PortalEncapsulatorSphere", BlueprintSpawnableComponent))
class PORTALS_API UPortalEncapsulatorSphere : public USphereComponent
{
	GENERATED_BODY()
		UPortalEncapsulatorSphere();
};
