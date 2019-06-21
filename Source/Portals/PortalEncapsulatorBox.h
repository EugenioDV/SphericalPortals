// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/BoxComponent.h"
#include "PortalEncapsulatorBox.generated.h"

/**
 * 
 */
UCLASS(ClassGroup = "Portal", hidecategories = (Object, LOD, Lighting, TextureStreaming), editinlinenew, meta = (DisplayName = "PortalEncapsulatorBox", BlueprintSpawnableComponent))
class PORTALS_API UPortalEncapsulatorBox : public UBoxComponent
{
	GENERATED_BODY()
		UPortalEncapsulatorBox();
};
