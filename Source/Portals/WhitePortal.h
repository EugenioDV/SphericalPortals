// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WhitePortal.generated.h"

class USceneCaptureComponent2D;
class USceneComponent;
class ABlackPortal;
class APortalManager;

UCLASS()
class PORTALS_API AWhitePortal : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AWhitePortal();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Visual)
		USceneCaptureComponent2D* SceneCapture;

	UPROPERTY(BlueprintReadOnly)
		USceneComponent* Root;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Setup)
		ABlackPortal* BlackPortal;

	/** The portal's radius, in Unreal Units */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Setup)
		float Radius = 400.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Setup)
		APortalManager* PortalManager;
};
