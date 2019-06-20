// Fill out your copyright notice in the Description page of Project Settings.


#include "PortalGameMode.h"
#include "PortalPlayerController.h"

APortalGameMode::APortalGameMode(const FObjectInitializer& ObjectInitializer) 
	: Super(ObjectInitializer.DoNotCreateDefaultSubobject(TEXT("Sprite")))
{
	PlayerControllerClass = APortalPlayerController::StaticClass();
}
