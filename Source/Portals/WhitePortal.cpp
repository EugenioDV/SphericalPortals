// Fill out your copyright notice in the Description page of Project Settings.


#include "WhitePortal.h"
#include "BlackPortal.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "Kismet/KismetSystemLibrary.h"


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

void AWhitePortal::MakeTheThing()
{
	FVector ConeApex, ConeDir;
	float ConeMinHeight, ConeCos;

	ConeApex = SceneCapture->GetComponentLocation();

	ConeDir = ConeApex - GetActorLocation();
	ConeMinHeight = ConeDir.Size();
	ConeDir /= ConeMinHeight;

	if (true) { //variables are only declared for clarity, they are not to be used anywhere else
		FVector2D EdgeOffset = FVector2D(ConeDir.Y, -ConeDir.X); //this is cheaper than cross product for obtaining a perpendicular vector. A future me can handle finding one that's also normalized
		EdgeOffset.Normalize();
		FVector EdgePoint = (GetActorLocation() + FVector(EdgeOffset.X * Radius, EdgeOffset.Y * Radius, 0.f));
		FVector PointToEdge = (EdgePoint - ConeApex);
		PointToEdge.Normalize();

		ConeCos = FVector::DotProduct(ConeDir, PointToEdge);
	}
	 /*cone has been made. The four parameters of doom have been created*/

	UKismetSystemLibrary::DrawDebugCone(this, ConeApex, ConeDir, 99999.f, acos(ConeCos), acos(ConeCos), 7, FLinearColor::Blue);


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

