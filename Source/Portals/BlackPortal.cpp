// Fill out your copyright notice in the Description page of Project Settings.


#include "BlackPortal.h"
#include "WhitePortal.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"


// Sets default values
ABlackPortal::ABlackPortal()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject <USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	PortalStaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PortalStaticMesh"));
	PortalStaticMesh->SetupAttachment(Root);
	PortalStaticMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	CollisionSphere = CreateDefaultSubobject <USphereComponent>(TEXT("OuterCollision"));
	CollisionSphere->SetupAttachment(Root);

	ConstructPortal();
}

// Called when the game starts or when spawned
void ABlackPortal::BeginPlay()
{
	Super::BeginPlay();
	
}

void ABlackPortal::ConstructPortal()
{
	PortalStaticMesh->SetRelativeScale3D(FVector(Radius / 200.f)); //this is tailored to the mesh import size! Watch out!
	CollisionSphere->SetSphereRadius(Radius);
	if (WhitePortal)
	{
		if (WhitePortal->Radius != Radius) {
			WhitePortal->Radius = Radius;
			WhitePortal->ConstructPortal();
		}
	}

}

// Called every frame
void ABlackPortal::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

