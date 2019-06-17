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

	void UpdatePortalRender(FVector Location, FRotator Rotation);

private:

	void BuildActorsRenderList(TArray<AActor*>* Candidates);

	FVector RenderConeVertex, RenderConeDir;
	float RenderConeMinHeight, RenderConeCos;

	void BuildRenderCone();

	void CalculateBoxInterval(FBox Box, float& Min, float& Max);

	bool BoxIntersectsConeDir(FBox Box);

	struct FBoxEdgePoint {//could use FIntPoint but meh
		int X, Y;
	};

	FBoxEdgePoint GlobalEdges[12] = {
		{0,1}, {1,3}, {2,3}, {0,2}, {4,5}, {5,7},
		{6,7}, {4,6}, {0,4}, {1,5}, {3,7}, {2,6}
	};

	bool CandidatesHavePointInsideCone(FVector Vertices[8]);

	bool EdgeHasPointInsideCone(FVector P0, FVector P1);

	bool IntersectingCandidatesHavePointInsideCone(FVector Vertices[8], float PMin[8]);
};
