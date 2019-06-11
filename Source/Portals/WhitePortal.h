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

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void UpdatePortalRender(FVector RefLocation, FRotator RefRotation);

	UFUNCTION(BlueprintCallable)
		void ConstructPortal();
};
