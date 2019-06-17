
// Fill out your copyright notice in the Description page of Project Settings.


#include "PortalPlayerController.h"
#include "BlackPortal.h"
#include "Kismet/GameplayStatics.h"
#include "SceneView.h "
#include "Engine/LocalPlayer.h"

float APortalPlayerController::GetPortalScreenRadius(ABlackPortal* Portal, APlayerCameraManager* CameraManager) {

	ULocalPlayer* LocalPlayer = GetLocalPlayer();
	bool bIsPortalInFrustum;
	if (LocalPlayer != nullptr && LocalPlayer->ViewportClient != nullptr && LocalPlayer->ViewportClient->Viewport !=nullptr)
	{
		FSceneViewFamilyContext ViewFamily(FSceneViewFamily::ConstructionValues(
			LocalPlayer->ViewportClient->Viewport,
			GetWorld()->Scene,
			LocalPlayer->ViewportClient->EngineShowFlags).SetRealtimeUpdate(true)		
		);

		FVector ViewLocation;
		FRotator ViewRotation;
		FSceneView* SceneView = LocalPlayer->CalcSceneView(&ViewFamily, ViewLocation, ViewRotation, LocalPlayer->ViewportClient->Viewport);
		if (SceneView != nullptr) bIsPortalInFrustum = SceneView->ViewFrustum.IntersectSphere(Portal->GetActorLocation(), Portal->Radius);
	}

	if (!bIsPortalInFrustum) return -1.f;


	if (!CameraManager) return -1.f;
	float CamFOV = FMath::DegreesToRadians(CameraManager->GetFOVAngle());

	FVector DirectionToObject = (Portal->GetActorLocation() - CameraManager->GetTransformComponent()->GetComponentLocation());

	float DistanceToObject = DirectionToObject.Size();
	DirectionToObject.Normalize();

	if (DistanceToObject < Portal->Radius*.45f) return 1.f; //if we are inside the portal, no need to do stuff. .45 because we want a little tolerance, we are tolerant people after all

	return FMath::Atan(Portal->Radius / (DistanceToObject*CamFOV));
}

