// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WhitePortal.generated.h"

class USceneCaptureComponent2D;
class USphereComponent;
class UStaticMeshComponent;
class ABlackPortal;

UCLASS()
class PORTALS_API AWhitePortal : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AWhitePortal();

	/** The portal's radius, in Unreal Units */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Setup)
		float Radius = 200.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Setup)
		ABlackPortal* BlackPortal;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Visual)
		USceneCaptureComponent2D* SceneCapture;

	UPROPERTY(BlueprintReadOnly)
		USceneComponent* Root;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly)
		UStaticMeshComponent* PortalMesh;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = Mechanics)
		USphereComponent* CollisionSphere;

private:
	void MakeTheThing();

	void BuildActorsRenderList(TArray<AActor*> Candidates);

	FVector RenderConeVertex, RenderConeDir;
	float RenderConeMinHeight, RenderConeCos;

	void BuildRenderCone();

	void CalculateBoxInterval(FBox Box, float& Min, float& Max);

	bool BoxIntersectsConeDir(FBox Box);

	struct FBoxEdgePoint {
		int8 X, Y;
	};

	struct FBoxFacePoint {
		int8 X, Y, Z, W;
	};

	struct FBoxGlobalFace {
		FBoxFacePoint One;
		FBoxFacePoint Two;
	};

	FBoxEdgePoint GlobalEdges[12] = {
		{0,1}, {1,3}, {2,3}, {0,2}, {4,5}, {5,7},
		{6,7}, {4,6}, {0,4}, {1,5}, {3,7}, {2,6}
	};

	FBoxGlobalFace GlobalFaces[6] = {
		{ {0,4,6,2}, {8,7,11,3} }, { {1,3,7,5}, {1,10,5,9} }, { {0,1,5,4}, {0,9,4,8} },
		{ {2,6,7,3}, {11,6,10,2} }, { {0,2,3,1}, {3,2,1,0} }, { {4,5,7,6}, {4,5,6,7} }
	};

	bool CandidatesHavePointInsideCone(FVector Vertices[32], int NumCandidates, FBoxEdgePoint Candidates[]);

	bool HasPointInsideCone(FVector P0, FVector P1);


public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void UpdatePortalRender(FVector RefLocation, FRotator RefRotation);

	UFUNCTION(BlueprintCallable)
		void ConstructPortal();
};
