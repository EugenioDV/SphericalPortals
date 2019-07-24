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

	CollisionSphere = CreateDefaultSubobject <USphereComponent>(TEXT("OuterCollision"));
	CollisionSphere->SetCollisionProfileName(TEXT("Portal"));
	SetRootComponent(CollisionSphere);

	PortalMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PortalMesh"));
	PortalMesh->SetupAttachment(CollisionSphere);
	PortalMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);


	SceneCapture = CreateDefaultSubobject<UPortalSceneCapture2D>(TEXT("SceneCaptureComponent"));
	SceneCapture->SetupAttachment(CollisionSphere);
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

void AWhitePortal::SetupPortalRender(const FVector &RefLocation, const FRotator &RefRotation, UTextureRenderTarget2D* RenderTarget)
{
	SceneCapture->TextureTarget = RenderTarget;

	SceneCapture->SetupPortalRender((RefLocation - BlackPortal->GetActorLocation()), RefRotation);
}

void AWhitePortal::RenderPortal()
{
	SceneCapture->RenderPortal();
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

	CollisionSphere->SetSphereRadius(Radius);
	PortalMesh->SetWorldScale3D(FVector(Radius * .005f));
}

void AWhitePortal::UpdateRenderTargetFOV(float NewFov)
{
	SceneCapture->FOVAngle = NewFov;
}

bool AWhitePortal::IsPortalSubportal(ABlackPortal* Candidate)
{
	UE_LOG(LogTemp, Warning, TEXT("Evaluating subportal..."));
	if (SceneCapture->SphereIntersectsRenderCone(Candidate->GetActorLocation(), Candidate->Radius)) {
		UE_LOG(LogTemp, Warning, TEXT("Subportal accepted!"));

	}


	return SceneCapture->SphereIntersectsRenderCone(Candidate->GetActorLocation(), Candidate->Radius);
}

