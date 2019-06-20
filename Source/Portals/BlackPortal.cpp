// Fill out your copyright notice in the Description page of Project Settings.


#include "BlackPortal.h"
#include "WhitePortal.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Materials/MaterialInstanceDynamic.h"

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

	
}

// Called when the game starts or when spawned
void ABlackPortal::BeginPlay()
{
	Super::BeginPlay();
	if (PortalBaseMaterial != nullptr) {
		PortalMaterialInstance = UMaterialInstanceDynamic::Create(PortalBaseMaterial, this);
		PortalStaticMesh->SetMaterial(0, PortalMaterialInstance);
	}
	else UE_LOG(LogTemp, Error, TEXT("Error! %s doesn't have a PortalBaseMaterial! Please set a default value in the class and verify that all your instances have a PortalBaseMaterial assigned"), *GetName());

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

void ABlackPortal::UpdateRenderTarget(UTextureRenderTarget2D* NewRenderTarget)
{
	if (PortalMaterialInstance != nullptr && NewRenderTarget != nullptr) PortalMaterialInstance->SetTextureParameterValue(TEXT("RenderTarget"), NewRenderTarget);

	else UE_LOG(LogTemp, Error, TEXT("Error! Attempting to update portal with invalid render target or material instance! %s"), *GetName());
}


// Called every frame
void ABlackPortal::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

