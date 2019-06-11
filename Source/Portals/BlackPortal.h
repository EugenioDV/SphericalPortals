// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BlackPortal.generated.h"

class USphereComponent;
class UStaticMeshComponent;
class AWhitePortal;

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

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = Mechanics)
		USphereComponent* CollisionSphere;

	UPROPERTY(BlueprintReadOnly)
		USceneComponent* Root;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;



public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(BlueprintReadOnly)
		float CurrentScreenSize;

	UFUNCTION(BlueprintCallable)
		void ConstructPortal();
};
