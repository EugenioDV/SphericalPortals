// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PortalManager.generated.h"

class ABlackPortal;
class AWhitePortal;


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

private:

	UPROPERTY(VisibleAnywhere, Category = Debug)
		TArray<ABlackPortal*> RelevantBlackPortals;

	UPROPERTY(VisibleAnywhere, Category = Debug)
		TArray<AWhitePortal*> RelevantWhitePortals;

	//minimum screen size for a portal to be considered for updating
	UPROPERTY(EditAnywhere, Category = Performance)
		float RelevantPortalScreenSize = .03f;

	//maximum number of portals being rendered in one frame
	UPROPERTY(EditAnywhere, Category = Performanc)
		int8 MaxActivePortals = 5;

	//resolution scale for a portal
	UPROPERTY(EditAnywhere, Category = Performance)
		float PortalResScale = 1.f;


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	void UpdatePortalRelevancy();
	void AddRelevantPortal(ABlackPortal* PortalToAdd);

};
