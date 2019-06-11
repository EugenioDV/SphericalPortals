// Fill out your copyright notice in the Description page of Project Settings.


#include "WhitePortal.h"
#include "BlackPortal.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"


//class APlayerCameraManager;

// Sets default values
AWhitePortal::AWhitePortal()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject <USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	PortalMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PortalMesh"));
	PortalMesh->SetupAttachment(Root);
	PortalMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	CollisionSphere = CreateDefaultSubobject <USphereComponent>(TEXT("OuterCollision"));
	CollisionSphere->SetupAttachment(Root);


	SceneCapture = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("SceneCaptureComponent"));
	SceneCapture->SetupAttachment(Root);
	SceneCapture->bEnableClipPlane = true;
	SceneCapture->bCaptureEveryFrame = false;
	SceneCapture->bCaptureOnMovement = false;

	this->SetTickGroup(ETickingGroup::TG_PostUpdateWork);
}

// Called when the game starts or when spawned
void AWhitePortal::BeginPlay()
{
	Super::BeginPlay();


	if (BlackPortal) {
		BlackPortal->WhitePortal = this;
	}
}

// Called every frame
void AWhitePortal::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AWhitePortal::UpdatePortalRender(FVector RefLocation, FRotator RefRotation)
{
	SceneCapture->SetRelativeLocation((RefLocation - BlackPortal->GetActorLocation()));
	SceneCapture->SetRelativeRotation(RefRotation);

	FVector PortalToCamera = Root->GetComponentLocation() - SceneCapture->GetComponentLocation();
	float CamDistance = PortalToCamera.Size();
	PortalToCamera /= CamDistance;

	SceneCapture->bEnableClipPlane = CamDistance > Radius; //disable clip plane if we are inside the portal

	SceneCapture->ClipPlaneBase = (Root->GetComponentLocation() - PortalToCamera * Radius);
	SceneCapture->ClipPlaneNormal = PortalToCamera;
	SceneCapture->CaptureSceneDeferred();
}


void AWhitePortal::ConstructPortal()
{
	if (BlackPortal) {
		BlackPortal->WhitePortal = this;
		if (BlackPortal->Radius != Radius) {
			BlackPortal->Radius = Radius;
			BlackPortal->ConstructPortal();
		}
	}

	PortalMesh->SetRelativeScale3D(FVector(Radius / 200.f)); //this is tailored to the mesh import size! Watch out!
	CollisionSphere->SetSphereRadius(Radius);
}

