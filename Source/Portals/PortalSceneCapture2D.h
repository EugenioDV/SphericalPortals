// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneCaptureComponent2D.h"
#include "PortalSceneCapture2D.generated.h"

class AWhitePortal;

/**
 * 
 */
UCLASS()
class PORTALS_API UPortalSceneCapture2D : public USceneCaptureComponent2D
{
	GENERATED_BODY()

	
public:
	AWhitePortal* WhitePortal;
	TArray<AActor*>* CandidatesForPortalRender;

	void UpdatePortalRender(const FVector &InRelativeLocation, const FRotator &InRelativeRotation);

private:

	void BuildActorsRenderList(TArray<AActor*>* Candidates);

	FVector RenderConeVertex, RenderConeDir;
	float RenderConeMinHeight, RenderConeCos;

	void BuildRenderCone();

	void CalculateBoxInterval(const FVector& BoxOrigin, const FVector& BoxExtent, float& OutMin, float& OutMax);

	bool BoxIntersectsConeDir(const FVector& BoxMin, const FVector& BoxMax);

	const FIntPoint GlobalEdges[12] = {
		{0,1}, {1,3}, {2,3}, {0,2}, {4,5}, {5,7},
		{6,7}, {4,6}, {0,4}, {1,5}, {3,7}, {2,6}
	};

	bool CandidatesHavePointInsideCone(const FVector Vertices[8]);

	bool EdgeHasPointInsideCone(const FVector &P0, const FVector &P1);

	bool IntersectingCandidatesHavePointInsideCone(const FVector Vertices[8]);
};
