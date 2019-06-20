// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PortalManager.generated.h"

class ABlackPortal;
class AWhitePortal;
class APortalPlayerController;
class APlayerCameraManager;
class UTextureRenderTarget2D;

UCLASS()
class PORTALS_API APortalManager : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	APortalManager();

	UPROPERTY(VisibleAnywhere, Category = Debug)
		TArray<AWhitePortal*> WhitePortals;

	UPROPERTY(VisibleAnywhere, Category = Debug)
		TArray<ABlackPortal*> BlackPortals;

	UPROPERTY(VisibleAnywhere, Category = Debug)
		TArray<AActor*> CandidatesForPortalRender;

	UPROPERTY(VisibleAnywhere, Category = Debug)
		APortalPlayerController* PortalController;

	UPROPERTY(VisibleAnywhere, Category = Debug)
		APlayerCameraManager* CameraManager;

private:

	UPROPERTY(VisibleAnywhere, Category = Debug)
		TArray<ABlackPortal*> RelevantBlackPortals;

	UPROPERTY(VisibleAnywhere, Category = Debug)
		TArray<AWhitePortal*> RelevantWhitePortals;

	UPROPERTY(VisibleAnywhere, Category = Debug)
		UTextureRenderTarget2D* RenderTargets[10]; //just in case, the array of pointer's length is equal to the maximum possible value for MaxActivePortals

	//minimum screen size for a portal to be considered for updating
	UPROPERTY(EditAnywhere, Category = Performance)
		float RelevantPortalScreenSize = .03f;

	//maximum number of portals being rendered in one frame
	UPROPERTY(EditAnywhere, Category = Performance, meta = (ClampMin = "1", ClampMax = "10", UIMin = "0", UIMax = "10"))
		uint8 MaxActivePortals = 5;


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	void UpdateFovAngle(const float& NewFov);

	UFUNCTION(BlueprintCallable)
	void UpdateRenderTargetTextures(const int& NewResX, const int& NewResY);

private:
	void UpdatePortalRelevancy();

	void AddRelevantPortal(ABlackPortal* PortalToAdd);

	//bad function, should be removed when we have stable conditions for resolution and fov changes 
	void DynamicPortalUpdate();

	int CurrentResX, CurrentResY, StoredResX, StoredResY;
	float StoredFovAngle;

	void BuildPortalRenderCandidatesList();
};
