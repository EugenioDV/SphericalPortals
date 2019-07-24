// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BlackPortal.generated.h"

class USphereComponent;
class UStaticMeshComponent;
class AWhitePortal;
class UTextureRenderTarget2D;
class UPortalEncapsulatorBox;
class UPortalEncapsulatorSphere;
class UMaterial;


UCLASS()
class PORTALS_API ABlackPortal : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABlackPortal();

	//set by White Portal's construct portal
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Setup)
		AWhitePortal* WhitePortal;

	//the radius of the portal, in Unreal Units
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Setup)
		float Radius = 200.f;


protected:

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly)
		UStaticMeshComponent* PortalStaticMesh;

	UPROPERTY(BlueprintReadOnly, Category = Mechanics)
		USphereComponent* CollisionSphere;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	//current material instance of the portal
	UPROPERTY(BlueprintReadOnly)
		UMaterialInstanceDynamic* PortalMaterialInstance;
	
	//the material to be instanced for the portal. Careful! Use PortalMaterialInstance to access runtime material stuff!
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Setup)
		UMaterial* PortalBaseMaterial;

private:

	UPROPERTY()
		TArray<UPortalEncapsulatorBox*> TeleportCandidateBoxes;

	UPROPERTY()
		TArray<UPortalEncapsulatorSphere*> TeleportCandidateSpheres;

	void TeleportActor(AActor* ActorToTeleport);

	UFUNCTION()
		void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
	UFUNCTION()
		void OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(BlueprintReadOnly)
		float CurrentScreenSize;

	//To be called in blueprint because constructor doesn't seem to work properly if called in CPP
	UFUNCTION(BlueprintCallable)
		void ConstructPortal();

	void UpdateRenderTarget(UTextureRenderTarget2D* NewRenderTarget);
};
