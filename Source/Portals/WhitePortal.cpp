// Fill out your copyright notice in the Description page of Project Settings.


#include "WhitePortal.h"
#include "BlackPortal.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Kismet/GameplayStatics.h"
#include "Camera/PlayerCameraManager.h"

//class APlayerCameraManager;

// Sets default values
AWhitePortal::AWhitePortal()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject <USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	SceneCapture = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("SceneCaptureComponent"));
	SceneCapture->SetupAttachment(Root);
	SceneCapture->bEnableClipPlane = true;

	this->SetTickGroup(ETickingGroup::TG_PostUpdateWork);

}

// Called when the game starts or when spawned
void AWhitePortal::BeginPlay()
{
	Super::BeginPlay();


	if (BlackPortal) {
		BlackPortal->WhitePortal = this;
		BlackPortal->PortalManager = PortalManager;
	}
}

// Called every frame
void AWhitePortal::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	APlayerCameraManager* CameraManager = UGameplayStatics::GetPlayerCameraManager(this, 0);

	SceneCapture->SetRelativeLocation((CameraManager->GetTransformComponent()->GetComponentLocation()) - BlackPortal->GetActorLocation());
	SceneCapture->SetWorldRotation(CameraManager->GetTransformComponent()->GetComponentRotation());
	SceneCapture->ClipPlaneBase = (Root->GetComponentLocation() - SceneCapture->GetForwardVector()*Radius*.5f);
	SceneCapture->ClipPlaneNormal = SceneCapture->GetForwardVector();

}

