// Fill out your copyright notice in the Description page of Project Settings.


#include "PortalSceneCapture2D.h"
#include "Engine/StaticMeshActor.h" //todo rogue code
#include "Engine/World.h" //todo rogue code
#include "Components/StaticMeshComponent.h" //todo rogue code

#include "WhitePortal.h"
#include "BlackPortal.h"
#include "Kismet/KismetSystemLibrary.h"

void UPortalSceneCapture2D::SetupPortalRender(const FVector &InRelativeLocation, const FRotator &InRelativeRotation)
{
	SetRelativeLocation(InRelativeLocation);
	SetRelativeRotation(InRelativeRotation);
	BuildRenderCone();

	//UKismetSystemLibrary::DrawDebugCone(this, RenderConeVertex, RenderConeDir, 999999.f, FMath::Acos(RenderConeCos), FMath::Acos(RenderConeCos), 30, FLinearColor::Blue);
}

void UPortalSceneCapture2D::RenderPortal()
{
	BuildActorsRenderList(CandidatesForPortalRender);

	for (AActor* ActorToHide : DuplicatedActors)
	{
		ActorToHide->SetActorHiddenInGame(true);
	}

	for (size_t a = 0; a< ShowOnlyActors.Num(); ++a)
	{
		
		AStaticMeshActor* Actor = Cast<AStaticMeshActor>(ShowOnlyActors[a]);

		if (!Actor) continue;

		FVector ActorLocation = Actor->GetActorLocation() + (WhitePortal->BlackPortal->GetActorLocation() - WhitePortal->GetActorLocation());

		FTransform Tr = FTransform(Actor->GetActorRotation(), ActorLocation, Actor->GetActorScale3D());

		DuplicatedActors[a]->SetActorTransform(Tr);
		DuplicatedActors[a]->SetActorHiddenInGame(false);

		DuplicatedActors[a]->GetStaticMeshComponent()->SetStaticMesh(Actor->GetStaticMeshComponent()->GetStaticMesh());
	}

	//CaptureSceneDeferred();
}

void UPortalSceneCapture2D::BeginPlay()
{
	Super::BeginPlay();

	for (size_t a = 0; a < 100; ++a)
	{
		FTransform Tr = FTransform(FRotator::ZeroRotator, FVector::ZeroVector, FVector::OneVector);

		FActorSpawnParameters Params;

		Params.Owner = GetOwner();
		Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		AStaticMeshActor* DuplicatedActor = GetWorld()->SpawnActor<AStaticMeshActor>(AStaticMeshActor::StaticClass(), Tr, Params);
		DuplicatedActor->GetStaticMeshComponent()->SetRenderInMainPass(false);
		DuplicatedActor->GetStaticMeshComponent()->SetRenderCustomDepth(true);
		//DuplicatedActor->GetStaticMeshComponent()->SetCustomDepthStencilValue(1);
		//DuplicatedActor->GetStaticMeshComponent()->SetCustomDepthStencilWriteMask(ERendererStencilMask::ERSM_255);

		DuplicatedActor->GetStaticMeshComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		DuplicatedActor->SetMobility(EComponentMobility::Movable);


		DuplicatedActors.Add(DuplicatedActor);
	}
}

void UPortalSceneCapture2D::BuildActorsRenderList(TArray<AActor*>* CandidateActors)
{
	if (WhitePortal->BlackPortal->CurrentScreenSize > .37f)
	{
		//if the portal is kinda big on screen, we skip the check and add all candidates
		ShowOnlyActors = *CandidateActors;
		return;
	}

	TArray<AActor*> Result = *new TArray<AActor*>;

	for (AActor* CurrentActor : *CandidateActors)
	{
		FVector BoxOrigin, BoxExtent, BoxMin, BoxMax;

		CurrentActor->GetActorBounds(false, BoxOrigin, BoxExtent);

		BoxMin = (BoxOrigin - BoxExtent);
		BoxMax = (BoxOrigin + BoxExtent);

		//UE_LOG(LogTemp, Warning, TEXT("[PortalSceneCapture2D] Building actor render list, current actor Actor: %s"), *CurrentActor->GetName()); 
		
		// box interval!
		float BoxMinHeight, BoxMaxHeight;
		CalculateBoxInterval(BoxOrigin, BoxExtent, BoxMinHeight, BoxMaxHeight);

		//quick dejectance test: box is beyond the plane that cuts our cone
		if (BoxMaxHeight >= 0)
		{
			//UE_LOG(LogTemp, Warning, TEXT("[PortalSceneCapture2D] Actor refusec quick dejectance: %s"), *CurrentActor->GetName());
			continue;
		}

		//quick acceptance test: box is intersected by cone direction ray
		if (BoxIntersectsConeDir(BoxMin, BoxMax))
		{
			//UKismetSystemLibrary::DrawDebugBox(this, BoxOrigin, BoxExtent, FLinearColor::Red, FRotator::ZeroRotator, 0.f, 10.f);
			Result.Add(CurrentActor);
			//UE_LOG(LogTemp, Warning, TEXT("[PortalSceneCapture2D]  Actor accepted quick acceptance: %s"), *CurrentActor->GetName());
			continue;
		}

		FVector Vertices[32];
		Vertices[0] = (FVector(BoxMin.X, BoxMin.Y, BoxMin.Z) - RenderConeVertex);
		Vertices[1] = (FVector(BoxMax.X, BoxMin.Y, BoxMin.Z) - RenderConeVertex);
		Vertices[2] = (FVector(BoxMin.X, BoxMax.Y, BoxMin.Z) - RenderConeVertex);
		Vertices[3] = (FVector(BoxMax.X, BoxMax.Y, BoxMin.Z) - RenderConeVertex);
		Vertices[4] = (FVector(BoxMin.X, BoxMin.Y, BoxMax.Z) - RenderConeVertex);
		Vertices[5] = (FVector(BoxMax.X, BoxMin.Y, BoxMax.Z) - RenderConeVertex);
		Vertices[6] = (FVector(BoxMin.X, BoxMax.Y, BoxMax.Z) - RenderConeVertex);
		Vertices[7] = (FVector(BoxMax.X, BoxMax.Y, BoxMax.Z) - RenderConeVertex);


		if (BoxMinHeight < 0)
		{
			//if box is fully inside the slope we check all candidates
			if (CandidatesHavePointInsideCone(Vertices))
			{
				Result.Add(CurrentActor);
				//UE_LOG(LogTemp, Warning, TEXT("[PortalSceneCapture2D] Actor accepted fully inside slope edge check: %s"), *CurrentActor->GetName());
				continue;
			}
			else
			{
				//UE_LOG(LogTemp, Warning, TEXT("[PortalSceneCapture2D] Actor fully inside slope, refused edge check: %s"), *CurrentActor->GetName());
				continue;
			}
		}

		//if box is not fully inside the slope, we find the section that is and arrange our data accordingly
		if (IntersectingCandidatesHavePointInsideCone(Vertices))
		{
			//UE_LOG(LogTemp, Warning, TEXT("[PortalSceneCapture2D] Actor accepted partially inside cone edge check: %s"), *CurrentActor->GetName());
			Result.Add(CurrentActor);
		}
		else
		{
			//UE_LOG(LogTemp, Warning, TEXT("[PortalSceneCapture2D] Actor refused partially inside cone edge check: %s"), *CurrentActor->GetName());
		}

	}

	ShowOnlyActors = Result;
}

void UPortalSceneCapture2D::BuildRenderCone()
{
	RenderConeVertex = GetComponentLocation();

	RenderConeDir = RenderConeVertex - WhitePortal->GetActorLocation();
	RenderConeMinHeight = RenderConeDir.Size();
	RenderConeDir /= RenderConeMinHeight;

	RenderConeEdgeOffset = FVector2D(RenderConeDir.Y, -RenderConeDir.X);
	RenderConeEdgeOffset.Normalize(); //getting a random vector perpendicular to the cone direction and normalizing it
	FVector EdgePoint = (WhitePortal->GetActorLocation() + FVector(RenderConeEdgeOffset.X, RenderConeEdgeOffset.Y, 0.f) * WhitePortal->Radius * 1.1f); //1.f is for a little tolerance
	FVector PointToEdge = (EdgePoint - RenderConeVertex);
	PointToEdge.Normalize();

	RenderConeCos = FVector::DotProduct(RenderConeDir, PointToEdge);
}


//This is a modified version of the Sphere-Cone interesection in the magnus engine. All credits go to Jonathan Hale, for more info, have a read! https://blog.squareys.de/dual-cone-view-culling-for-vr/
bool UPortalSceneCapture2D::SphereIntersectsRenderCone(const FVector& SphereCenter, float SphereRadius) {

	//quickDejectionTest for infinite cone frustum
	FVector coneNormal = -RenderConeDir;
	FVector diff = SphereCenter - RenderConeVertex;

	float distance = FVector::DotProduct(diff, coneNormal);

	if (distance < RenderConeMinHeight) return false;
	//actual test

	float halfAngle = FMath::Acos(RenderConeCos);


	float sinAngle = FMath::Sin(halfAngle);

	FVector c = sinAngle * diff + coneNormal * SphereRadius;
	float lenA = FVector::DotProduct(c, coneNormal);

	float tanAngleSqPlusOne = 1.f + FMath::Pow(FMath::Tan(halfAngle), 2.f);
	return c.SizeSquared() <= lenA * lenA * tanAngleSqPlusOne;
}

void UPortalSceneCapture2D::CalculateBoxInterval(const FVector &BoxOrigin, const FVector &BoxExtent, float& OutMin, float& OutMax)
{
	FVector CmV = (BoxOrigin - WhitePortal->GetActorLocation());
	float DdCmV = FVector::DotProduct(RenderConeDir, CmV);
	float CalcRadius = (BoxExtent.X * FMath::Abs(RenderConeDir.X) + BoxExtent.Y * FMath::Abs(RenderConeDir.Y) + BoxExtent.Z * FMath::Abs(RenderConeDir.Z));

	OutMin = DdCmV + CalcRadius;
	OutMax = DdCmV - CalcRadius;
}

bool UPortalSceneCapture2D::BoxIntersectsConeDir(const FVector &BoxMin, const FVector &BoxMax)
{
	const int NumDim = 3;
	const int Right = 0;
	const int Left = 1;
	const int Middle = 2;

	FVector coord;				/* hit point */
	{
		bool inside = true;
		char quadrant[NumDim];
		register int i;
		int whichPlane;
		double maxT[NumDim];
		double candidatePlane[NumDim];

		/* Find candidate planes; this loop can be avoided if
		rays cast all from the eye(assume perpsective view) */
		for (i = 0; i < NumDim; i++)
			if (WhitePortal->GetActorLocation()[i] < BoxMin[i]) {
				quadrant[i] = Left;
				candidatePlane[i] = BoxMin[i];
				inside = false;
			}
			else if (WhitePortal->GetActorLocation()[i] > BoxMax[i]) {
				quadrant[i] = Right;
				candidatePlane[i] = BoxMax[i];
				inside = false;
			}
			else {
				quadrant[i] = Middle;
			}

		/* Ray origin inside bounding box */
		if (inside) {
			coord = WhitePortal->GetActorLocation();
			return (true);
		}


		/* Calculate T distances to candidate planes */
		for (i = 0; i < NumDim; i++)
			if (quadrant[i] != Middle && -RenderConeDir[i] != 0.)
				maxT[i] = (candidatePlane[i] - WhitePortal->GetActorLocation()[i]) / -RenderConeDir[i];
			else
				maxT[i] = -1.;

		/* Get largest of the maxT's for final choice of intersection */
		whichPlane = 0;
		for (i = 1; i < NumDim; i++)
			if (maxT[whichPlane] < maxT[i])
				whichPlane = i;

		/* Check final candidate actually inside box */
		if (maxT[whichPlane] < 0.) return (false);
		for (i = 0; i < NumDim; i++)
			if (whichPlane != i) {
				coord[i] = WhitePortal->GetActorLocation()[i] + maxT[whichPlane] * -RenderConeDir[i];
				if (coord[i] < BoxMin[i] || coord[i] > BoxMax[i])
					return (false);
			}
			else {
				coord[i] = candidatePlane[i];
			}
		return (true);				/* ray hits box */
	}
}

bool UPortalSceneCapture2D::CandidatesHavePointInsideCone(const FVector Vertices[8])
{

	for (int i = 0; i < 12; ++i)
	{
		FIntPoint CurrentEdge = GlobalEdges[i];
		FVector P0 = Vertices[CurrentEdge.X];
		FVector P1 = Vertices[CurrentEdge.Y];

		if (EdgeHasPointInsideCone(P0, P1))	return true;
	}
	return false;
}

bool UPortalSceneCapture2D::EdgeHasPointInsideCone(const FVector &P0, const FVector &P1)
{
	//UKismetSystemLibrary::DrawDebugLine(this, P0 + RenderConeVertex, P1 + RenderConeVertex, FLinearColor::Green, 0.f, 10.f);

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

bool UPortalSceneCapture2D::IntersectingCandidatesHavePointInsideCone(const FVector Vertices[8])
{

	float PMin[8];

	for (int i = 0; i < 8; ++i)
	{
		float H = FVector::DotProduct(RenderConeDir, Vertices[i]);
		PMin[i] = RenderConeMinHeight + H;
	}


	for (int i = 0; i < 12; ++i)
	{
		FIntPoint CurrentEdge = GlobalEdges[i];

		float P0Min = PMin[CurrentEdge.X];
		float P1Min = PMin[CurrentEdge.Y];
		FVector P0, P1;

		if (P0Min > 0 && P1Min > 0) continue;	//none of the vertices was in the box, edge will not be checked

		if (P0Min < 0 && P1Min < 0)
		{	
			//both vertices of the edge are inside the slope, edge and vertices will be preserved
			P0 = Vertices[CurrentEdge.X];
			P1 = Vertices[CurrentEdge.Y];
		}

		if (P0Min < 0 && P1Min > 0)
		{
			//P1Min is outside of the slope: it must die
			P0 = Vertices[CurrentEdge.X];
			float Q = P0Min / (P1Min - P0Min);
			P1 = (Vertices[CurrentEdge.X] + ((Vertices[CurrentEdge.X] - Vertices[CurrentEdge.Y]) * Q));
		}

		if (P1Min < 0 && P0Min > 0)
		{
			//P0Min is outside of the slope: it must die
			P0 = Vertices[CurrentEdge.Y];
			float Q = P1Min / (P0Min - P1Min);
			P1 = (Vertices[CurrentEdge.Y] + ((Vertices[CurrentEdge.Y] - Vertices[CurrentEdge.X]) * Q));
		}

		if (EdgeHasPointInsideCone(P0, P1))	return true;

	}

	return false;
}