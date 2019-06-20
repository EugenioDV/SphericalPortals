// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "PortalPlayerController.generated.h"

class ABlackPortal;
/**
 * 
 */
UCLASS()
class PORTALS_API APortalPlayerController : public APlayerController
{
	GENERATED_BODY()

public:

	float GetPortalScreenSize(ABlackPortal * Portal, APlayerCameraManager* CameraManager);

	bool GetCurrentGameResolution(int& ResX, int& ResY);

};
