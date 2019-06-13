// Fill out your copyright notice in the Description page of Project Settings.


#include "WhitePortal.h"
#include "BlackPortal.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"

//might want to remove
#include "Kismet/KismetSystemLibrary.h"
#include "EngineUtils.h" 
#include "Engine/StaticMeshActor.h"


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
	SceneCapture->bCaptureEveryFrame = false;
	SceneCapture->bCaptureOnMovement = false;
	SceneCapture->PrimitiveRenderMode = ESceneCapturePrimitiveRenderMode::PRM_UseShowOnlyList;


	this->SetTickGroup(ETickingGroup::TG_PostUpdateWork);
}

// Called when the game starts or when spawned
void AWhitePortal::BeginPlay()
{
	Super::BeginPlay();


	if (BlackPortal) {
		BlackPortal->WhitePortal = this;
	}



	for (FBoxEdgePoint EdgePoint : GlobalEdges) {
		//UE_LOG(LogTemp, Warning, TEXT("Edgepoint X: %d Y: %d"), EdgePoint.X, EdgePoint.Y);
	}

	for (FBoxGlobalFace Face : GlobalFaces) {
		//UE_LOG(LogTemp, Warning, TEXT("GobalFace X: %d Y: %d"), Face.One.X, Face.One.Y);
	}
}

void AWhitePortal::MakeTheThing()
{

	TArray <AActor*> Candidates = *new TArray<AActor*>;
	for (TActorIterator<AStaticMeshActor> ActorItr(GetWorld()); ActorItr; ++ActorItr){
		Candidates.Add((AActor*)*ActorItr);
	}

	BuildActorsRenderList(Candidates);
}

void AWhitePortal::BuildActorsRenderList(TArray<AActor*> CandidateActors)
{
	TArray<AActor*> Result = *new TArray<AActor*>;

	for (AActor* CurrentActor : CandidateActors) {

		FBox Box = CurrentActor->GetComponentsBoundingBox();

		// box interval!
		float BoxMinHeight, BoxMaxHeight;
		CalculateBoxInterval(Box, BoxMinHeight, BoxMaxHeight);

		//quick dejectance test: box is beyond the plane that cuts our cone
		if (BoxMaxHeight >= 0) 	continue;

		//quick acceptance test: box is intersected by cone direction ray
		if (BoxIntersectsConeDir(Box)) {
			Result.Add(CurrentActor);
			continue;
		}

		FVector Vertices[32];
		Vertices[0] = (FVector(Box.Min.X, Box.Min.Y, Box.Min.Z) - RenderConeVertex);
		Vertices[1] = (FVector(Box.Max.X, Box.Min.Y, Box.Min.Z) - RenderConeVertex);
		Vertices[2] = (FVector(Box.Min.X, Box.Max.Y, Box.Min.Z) - RenderConeVertex);
		Vertices[3] = (FVector(Box.Max.X, Box.Max.Y, Box.Min.Z) - RenderConeVertex);
		Vertices[4] = (FVector(Box.Min.X, Box.Min.Y, Box.Max.Z) - RenderConeVertex);
		Vertices[5] = (FVector(Box.Max.X, Box.Min.Y, Box.Max.Z) - RenderConeVertex);
		Vertices[6] = (FVector(Box.Min.X, Box.Max.Y, Box.Max.Z) - RenderConeVertex);
		Vertices[7] = (FVector(Box.Max.X, Box.Max.Y, Box.Max.Z) - RenderConeVertex);

		FBoxEdgePoint Candidates[32 * 32];

		if (BoxMinHeight < 0) {
			//if box is fully inside the slope, we fill the candidates array with the box vertices and perform our checks
			for (int i = 0; i < 12; ++i) Candidates[i] = GlobalEdges[i];

			if (CandidatesHavePointInsideCone(Vertices, 12, Candidates)) {
				Result.Add(CurrentActor);
				continue;
			}
			else continue;
		}

		//if box is not fully inside the slope, we find the section that is and arrange our data accordingly
		Result.Add(CurrentActor);
		continue;

	}

	for (AActor* CurrentActor : Result) {

		FBox Box = CurrentActor->GetComponentsBoundingBox();
		UKismetSystemLibrary::DrawDebugBox(this, Box.GetCenter(), Box.GetExtent(), FLinearColor::Blue, FRotator::ZeroRotator, 0.f, 4.f);
	}



	SceneCapture->ShowOnlyActors = Result;
}

void AWhitePortal::BuildRenderCone()
{

	RenderConeVertex = SceneCapture->GetComponentLocation();

	RenderConeDir = RenderConeVertex - GetActorLocation();
	RenderConeMinHeight = RenderConeDir.Size();
	RenderConeDir /= RenderConeMinHeight;

	FVector2D EdgeOffset = FVector2D(RenderConeDir.Y, -RenderConeDir.X);
	EdgeOffset.Normalize(); //getting a random vector perpendicular to the cone direction and normalizing it
	FVector EdgePoint = (GetActorLocation() + FVector(EdgeOffset.X, EdgeOffset.Y, 0.f)*Radius*1.05f); //1.05 is for a little tolerance
	FVector PointToEdge = (EdgePoint - RenderConeVertex);
	PointToEdge.Normalize();

	RenderConeCos = FVector::DotProduct(RenderConeDir, PointToEdge);

	UKismetSystemLibrary::DrawDebugCone(this, RenderConeVertex, RenderConeDir, 99999.f, acos(RenderConeCos), acos(RenderConeCos), 20, FLinearColor::Blue); //todo remove and kismet include
}

void AWhitePortal::CalculateBoxInterval(FBox Box, float& Min, float& Max)
{
	FVector BoxExtent = Box.GetExtent();
	FVector CmV = (Box.GetCenter() - GetActorLocation());
	float DdCmV = FVector::DotProduct(RenderConeDir, CmV);
	float CalcRadius = (BoxExtent.X * FMath::Abs(RenderConeDir.X) + BoxExtent.Y * FMath::Abs(RenderConeDir.Y) + BoxExtent.Z * FMath::Abs(RenderConeDir.Z));

	Min = DdCmV + CalcRadius;
	Max = DdCmV - CalcRadius;
}

bool AWhitePortal::BoxIntersectsConeDir(FBox Box)
{
	float tmin, tmax;

	for (int i = 0; i < 3; ++i) {
		float InvD = 1.0f / RenderConeDir[i];
		float t0 = ((Box.Min[i] - GetActorLocation()[i]) * InvD);
		float t1 = ((Box.Max[i] - GetActorLocation()[i]) * InvD);

		if (InvD < 0.0f) { //switch the values around
			t1 += t0;
			t0 = t1 - t0;
			t1 -= t0;
		}

		if (i == 0) {
			tmin = t0, tmax = t1;
		}

		else {
			tmin = t0 > tmin ? t0 : tmin;
			tmax = t1 < tmax ? t1 : tmax;
		}

		if (tmax <= tmin)	return false;	// false
	}
	return true;
}

bool AWhitePortal::CandidatesHavePointInsideCone(FVector Vertices[32], int NumCandidates, FBoxEdgePoint Candidates[])
{

	for (int i = 0; i < NumCandidates; ++i) {
		FBoxEdgePoint CurrentEdge = Candidates[i];

		FVector P0 = Vertices[CurrentEdge.X];
		FVector P1 = Vertices[CurrentEdge.Y];

		if (HasPointInsideCone(P0, P1))	return true;
	}
	return false;
}

bool AWhitePortal::HasPointInsideCone(FVector P0, FVector P1)
{

	//If one of the two edge points is inside the cone, we are done
	if (FVector::DotProduct(P0, RenderConeDir) < RenderConeCos * P0.Size()) return true;
	if (FVector::DotProduct(P1, RenderConeDir) < RenderConeCos * P1.Size()) return true;
	//Otherwise, we look for the maximizer in the segment between the two points
	
	FVector E = P1 - P0;
	FVector CrossP0U = FVector::CrossProduct(P0, RenderConeDir);
	FVector CrossP0E = FVector::CrossProduct(P0, E);

	float DPHI0 = FVector::DotProduct(CrossP0E, CrossP0U);
	if (DPHI0 > 0) return false;

	FVector CrossP1U = FVector::CrossProduct(P1, RenderConeDir);
	float DPHI1 = FVector::DotProduct(CrossP0E, CrossP1U);
	if (DPHI1 < 0) return false;
		
	float T = DPHI0 / (DPHI0 - DPHI1);
	FVector PMax = P0 + T * E;
	return FVector::DotProduct(RenderConeDir, PMax) < RenderConeCos * PMax.Size(); // the maximizer being inside the cone will determine the result of this test
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

	BuildRenderCone();
	MakeTheThing(); //todo remove

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

