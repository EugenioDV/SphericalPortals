// Fill out your copyright notice in the Description page of Project Settings.


#include "PortalSceneCapture2D.h"
#include "WhitePortal.h"
#include "BlackPortal.h"
#include "Kismet/KismetSystemLibrary.h"

void UPortalSceneCapture2D::UpdatePortalRender(FVector InRelativeLocation, FRotator InRelativeRotation)
{
	SetRelativeLocation(InRelativeLocation);
	SetRelativeRotation(InRelativeRotation);
	
	BuildRenderCone();
	UKismetSystemLibrary::DrawDebugCone(this, RenderConeVertex, RenderConeDir, 999999.f, FMath::Acos(RenderConeCos), FMath::Acos(RenderConeCos), 10, FLinearColor::Blue);
	BuildActorsRenderList(CandidatesForPortalRender);
	CaptureSceneDeferred();

}

void UPortalSceneCapture2D::BuildActorsRenderList(TArray<AActor*>* CandidateActors)
{
	if (WhitePortal->BlackPortal->CurrentScreenSize > .37f) { //if the portal is kinda big on screen, we skip the check
		ShowOnlyActors = *CandidateActors;
		return;
	}

	TArray<AActor*> Result = *new TArray<AActor*>;

	for (AActor* CurrentActor : *CandidateActors) {
		
		FVector BoxOrigin, BoxExtent, BoxMin, BoxMax;

		CurrentActor->GetActorBounds(false, BoxOrigin, BoxExtent);

		BoxMin = (BoxOrigin - BoxExtent);
		BoxMax = (BoxOrigin + BoxExtent);

		//UKismetSystemLibrary::DrawDebugBox(this, BoxOrigin, BoxExtent, FLinearColor::Black, FRotator::ZeroRotator, 0.f, 10.f);
		//UE_LOG(LogTemp, Warning, TEXT("[PortalSceneCapture2D] Building actor render list, current actor Actor: %s"), *CurrentActor->GetName()); 
		
		// box interval!
		float BoxMinHeight, BoxMaxHeight;
		CalculateBoxInterval(BoxOrigin, BoxExtent, BoxMinHeight, BoxMaxHeight);

		//quick dejectance test: box is beyond the plane that cuts our cone
		if (BoxMaxHeight >= 0) {
			//UE_LOG(LogTemp, Warning, TEXT("[PortalSceneCapture2D] Actor refusec quick dejectance: %s"), *CurrentActor->GetName());
			continue;
		}

		//quick acceptance test: box is intersected by cone direction ray
		if (BoxIntersectsConeDir(BoxMin, BoxMax)) {
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


		if (BoxMinHeight < 0) {
			//if box is fully inside the slope we check all candidates
			if (CandidatesHavePointInsideCone(Vertices)) {
				Result.Add(CurrentActor);
				//UE_LOG(LogTemp, Warning, TEXT("[PortalSceneCapture2D] Actor accepted fully inside slope edge check: %s"), *CurrentActor->GetName());
				continue;
			}
			else {
				//UE_LOG(LogTemp, Warning, TEXT("[PortalSceneCapture2D] Actor fully inside slope, refused edge check: %s"), *CurrentActor->GetName());
				continue;
			}
		}

		//if box is not fully inside the slope, we find the section that is and arrange our data accordingly
		if (IntersectingCandidatesHavePointInsideCone(Vertices)) {
			//UE_LOG(LogTemp, Warning, TEXT("[PortalSceneCapture2D] Actor accepted partially inside cone edge check: %s"), *CurrentActor->GetName());
			Result.Add(CurrentActor);
		}
		else {
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

	FVector2D EdgeOffset = FVector2D(RenderConeDir.Y, -RenderConeDir.X);
	EdgeOffset.Normalize(); //getting a random vector perpendicular to the cone direction and normalizing it
	FVector EdgePoint = (WhitePortal->GetActorLocation() + FVector(EdgeOffset.X, EdgeOffset.Y, 0.f) * WhitePortal->Radius * 1.1f); //1.f is for a little tolerance
	FVector PointToEdge = (EdgePoint - RenderConeVertex);
	PointToEdge.Normalize();

	RenderConeCos = FVector::DotProduct(RenderConeDir, PointToEdge);
}

void UPortalSceneCapture2D::CalculateBoxInterval(FVector &BoxOrigin, FVector &BoxExtent, float& Min, float& Max)
{
	FVector CmV = (BoxOrigin - WhitePortal->GetActorLocation());
	float DdCmV = FVector::DotProduct(RenderConeDir, CmV);
	float CalcRadius = (BoxExtent.X * FMath::Abs(RenderConeDir.X) + BoxExtent.Y * FMath::Abs(RenderConeDir.Y) + BoxExtent.Z * FMath::Abs(RenderConeDir.Z));

	Min = DdCmV + CalcRadius;
	Max = DdCmV - CalcRadius;
}

bool UPortalSceneCapture2D::BoxIntersectsConeDir(FVector &BoxMin, FVector &BoxMax)
{
	float t0max = -BIG_NUMBER, t0min = BIG_NUMBER;

	for (int i = 0; i < 3; ++i) {
		float InvD = 1.0f / RenderConeDir[i];
		float t0 = ((BoxMin[i] - WhitePortal->GetActorLocation()[i]) * InvD);
		float t1 = ((BoxMax[i] - WhitePortal->GetActorLocation()[i]) * InvD);

		if (InvD < 0.0f) { //switch the values around
			t1 += t0;
			t0 = t1 - t0;
			t1 -= t0;
		}

		t0max = t0 > t0max ? t0 : t0max;
		t0min = t1 < t0min ? t1 : t0min;

		if (t0min <= t0max)	return false;
	}
	return true;
}

bool UPortalSceneCapture2D::CandidatesHavePointInsideCone(FVector Vertices[8])
{

	for (int i = 0; i < 12; ++i) {
		FIntPoint CurrentEdge = GlobalEdges[i];
		FVector P0 = Vertices[CurrentEdge.X];
		FVector P1 = Vertices[CurrentEdge.Y];

		if (EdgeHasPointInsideCone(P0, P1))	return true;
	}
	return false;
}

bool UPortalSceneCapture2D::EdgeHasPointInsideCone(FVector P0, FVector P1)
{
	UKismetSystemLibrary::DrawDebugLine(this, P0 + RenderConeVertex, P1 + RenderConeVertex, FLinearColor::Green, 0.f, 10.f);

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

bool UPortalSceneCapture2D::IntersectingCandidatesHavePointInsideCone(FVector Vertices[8])
{

	float PMin[8];

	for (int i = 0; i < 8; ++i) {
		float H = FVector::DotProduct(RenderConeDir, Vertices[i]);
		PMin[i] = RenderConeMinHeight + H;
	}


	for (int i = 0; i < 12; ++i) {
		FIntPoint CurrentEdge = GlobalEdges[i];

		float P0Min = PMin[CurrentEdge.X];
		float P1Min = PMin[CurrentEdge.Y];
		FVector P0, P1;

		if (P0Min > 0 && P1Min > 0) continue;	//none of the vertices was in the box, edge will not be checked

		if (P0Min < 0 && P1Min < 0) {		//both vertices of the edge are inside the slope, edge and vertices will be preserved
			P0 = Vertices[CurrentEdge.X];
			P1 = Vertices[CurrentEdge.Y];
		}

		if (P0Min < 0 && P1Min > 0) {	//P1Min must di
			P0 = Vertices[CurrentEdge.X];
			float Q = P0Min / (P1Min - P0Min);
			P1 = (Vertices[CurrentEdge.X] + ((Vertices[CurrentEdge.X] - Vertices[CurrentEdge.Y]) * Q));
		}

		if (P1Min < 0 && P0Min > 0) {	//P0Min must die
			P0 = Vertices[CurrentEdge.Y];
			float Q = P1Min / (P0Min - P1Min);
			P1 = (Vertices[CurrentEdge.Y] + ((Vertices[CurrentEdge.Y] - Vertices[CurrentEdge.X]) * Q));
		}

		if (EdgeHasPointInsideCone(P0, P1))	return true;

	}

	return false;
}