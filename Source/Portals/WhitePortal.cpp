// Fill out your copyright notice in the Description page of Project Settings.


#include "WhitePortal.h"
#include "BlackPortal.h"
#include "PortalSceneCapture2D.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"


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


	SceneCapture = CreateDefaultSubobject<UPortalSceneCapture2D>(TEXT("SceneCaptureComponent"));
	SceneCapture->SetupAttachment(Root);
	//might want to transfer this stuff to PortalComponent construct
	SceneCapture->bCaptureEveryFrame = false;
	SceneCapture->bCaptureOnMovement = false;
	SceneCapture->PrimitiveRenderMode = ESceneCapturePrimitiveRenderMode::PRM_UseShowOnlyList;
	SceneCapture->WhitePortal = this;


	this->SetTickGroup(ETickingGroup::TG_PostUpdateWork);
}

// Called when the game starts or when spawned
void AWhitePortal::BeginPlay()
{
	Super::BeginPlay();


	if (BlackPortal)
	{
		BlackPortal->WhitePortal = this;
	}

}



// Called every frame
void AWhitePortal::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AWhitePortal::UpdatePortalRender(const FVector &RefLocation, const FRotator &RefRotation, UTextureRenderTarget2D* RenderTarget)
{
	SceneCapture->TextureTarget = RenderTarget;

	SceneCapture->UpdatePortalRender((RefLocation - BlackPortal->GetActorLocation()), RefRotation);
}

void AWhitePortal::UpdateRenderCandidates(TArray<AActor*>* NewRenderCandidates)
{
	SceneCapture->CandidatesForPortalRender = NewRenderCandidates;
}

void AWhitePortal::ConstructPortal()
{
	if (BlackPortal)
	{
		BlackPortal->WhitePortal = this;
		if (BlackPortal->Radius != Radius)
		{
			BlackPortal->Radius = Radius;
			BlackPortal->ConstructPortal();
		}
	}

	PortalMesh->SetRelativeScale3D(FVector(Radius / 200.f)); //this is tailored to the mesh import size! Watch out!
	CollisionSphere->SetSphereRadius(Radius);
}

void AWhitePortal::UpdateRenderTargetFOV(const float& NewFov)
{
	SceneCapture->FOVAngle = NewFov;
}

