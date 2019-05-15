// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BlackPortal.generated.h"

class USphereComponent;
class UStaticMeshComponent;
class USceneComponent;
class AWhitePortal;
class APortalManager;

UCLASS()
class PORTALS_API ABlackPortal : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABlackPortal();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		UStaticMeshComponent* MeshHole;

	UPROPERTY(BlueprintReadOnly, Category = Mechanics)
		USphereComponent* OuterCollision;

	UPROPERTY(BlueprintReadOnly, Category = Mechanics)
		USphereComponent* InnerCollision;

	UPROPERTY(BlueprintReadOnly)
		USceneComponent* Root;

	UPROPERTY(BlueprintReadWrite, Category = Setup)
		AWhitePortal* WhitePortal;

	UPROPERTY(BlueprintReadWrite, Category = Setup)
		APortalManager* PortalManager;
	   
};
