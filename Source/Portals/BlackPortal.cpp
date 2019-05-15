// Fill out your copyright notice in the Description page of Project Settings.


#include "BlackPortal.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"


// Sets default values
ABlackPortal::ABlackPortal()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject <USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	MeshHole = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshHole"));
	MeshHole->SetupAttachment(Root);
	MeshHole->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	OuterCollision = CreateDefaultSubobject <USphereComponent>(TEXT("OuterCollision"));
	OuterCollision->SetupAttachment(Root);

	InnerCollision = CreateDefaultSubobject <USphereComponent>(TEXT("InnerCollision"));
	InnerCollision->SetupAttachment(Root);

	OuterCollision->SetSphereRadius(MeshHole->RelativeScale3D.X*200.f);
	InnerCollision->SetSphereRadius(MeshHole->RelativeScale3D.X*200.f-10.f);
}

// Called when the game starts or when spawned
void ABlackPortal::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABlackPortal::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

