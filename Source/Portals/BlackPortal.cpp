// Fill out your copyright notice in the Description page of Project Settings.


#include "BlackPortal.h"
#include "WhitePortal.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "PortalEncapsulatorBox.h"
#include "PortalEncapsulatorSphere.h"
#include "PortalInterface.h"

// Sets default values
ABlackPortal::ABlackPortal()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CollisionSphere = CreateDefaultSubobject <USphereComponent>(TEXT("OuterCollision"));
	CollisionSphere->SetCollisionProfileName(TEXT("Portal"));
	SetRootComponent(CollisionSphere);

	PortalStaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PortalStaticMesh"));
	PortalStaticMesh->SetupAttachment(CollisionSphere);
	PortalStaticMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	
	SetActorTickEnabled(false);
	
}

// Called when the game starts or when spawned
void ABlackPortal::BeginPlay()
{
	CollisionSphere->OnComponentBeginOverlap.AddDynamic(this, &ABlackPortal::OnOverlapBegin);
	CollisionSphere->OnComponentEndOverlap.AddDynamic(this, &ABlackPortal::OnOverlapEnd);

	Super::BeginPlay();
	if (PortalBaseMaterial != nullptr)
	{
		PortalMaterialInstance = UMaterialInstanceDynamic::Create(PortalBaseMaterial, this);
		PortalStaticMesh->SetMaterial(0, PortalMaterialInstance);
	}
	else UE_LOG(LogTemp, Error, TEXT("Error! %s doesn't have a PortalBaseMaterial! Please set a default value in the class and verify that all your instances have a PortalBaseMaterial assigned"), *GetName());

}

void ABlackPortal::ConstructPortal()
{
	PortalStaticMesh->SetRelativeScale3D(FVector(Radius * .005f)); //this is tailored to the mesh import size! Watch out!
	CollisionSphere->SetSphereRadius(Radius);
	if (WhitePortal)
	{
		if (WhitePortal->Radius != Radius)
		{
			WhitePortal->Radius = Radius;
			WhitePortal->ConstructPortal();
		}
	}

}

void ABlackPortal::UpdateRenderTarget(UTextureRenderTarget2D* NewRenderTarget)
{
	if (!PortalMaterialInstance)
	{
		UE_LOG(LogTemp, Error, TEXT("Error! Attempting to update portal with invalid material instance! %s"), *GetName());
		return;
	}
		
	if (!NewRenderTarget)
	{
		UE_LOG(LogTemp, Error, TEXT("Error! Attempting to update portal with invalid render target! %s"), *GetName());
		return;
	}

	PortalMaterialInstance->SetTextureParameterValue(TEXT("RenderTarget"), NewRenderTarget);
}



// Called every frame
void ABlackPortal::Tick(float DeltaTime)
{

	Super::Tick(DeltaTime);

	TArray<int> TeleportedActorIndexes;

	for (UPortalEncapsulatorBox* CurrentComp : TeleportCandidateBoxes)
	{

	}

	TeleportedActorIndexes.Empty();
	for (int i = 0; i<TeleportCandidateSpheres.Num(); ++i)
	{
		UPortalEncapsulatorSphere* CurrentComp = TeleportCandidateSpheres[i];
		float Distance = (CurrentComp->GetComponentLocation() - GetActorLocation()).Size();

		if (Distance + CurrentComp->GetScaledSphereRadius() < CollisionSphere->GetScaledSphereRadius())
		{
			TeleportActor(CurrentComp->GetOwner());
			TeleportedActorIndexes.Add(i);
		}
	}
	for (int RemovalIndex : TeleportedActorIndexes)
	{
	//	TeleportCandidateSpheres.RemoveAt(RemovalIndex);
	}



	if (TeleportCandidateBoxes.Num() == 0 && TeleportCandidateSpheres.Num() == 0) SetActorTickEnabled(false);

}

void ABlackPortal::TeleportActor(AActor* ActorToTeleport)
{
	FRotator DeltaRotation = WhitePortal->GetActorRotation();

	FVector DeltaLocation = DeltaRotation.RotateVector(ActorToTeleport->GetActorLocation() - GetActorLocation());

	ActorToTeleport->SetActorLocation(DeltaLocation+WhitePortal->GetActorLocation());
	ActorToTeleport->AddActorWorldRotation(DeltaRotation);

	if (ActorToTeleport->Implements<UPortalInterface>())
	{
		IPortalInterface::Execute_ReactToTeleportation(ActorToTeleport, DeltaRotation, this);
	}

}

void ABlackPortal::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{

	UPortalEncapsulatorBox* Box = Cast<UPortalEncapsulatorBox>(OtherComp);

	if (Box != nullptr)
	{
		TeleportCandidateBoxes.Add(Box);
		SetActorTickEnabled(true);
	}

	UPortalEncapsulatorSphere* Sphere = Cast<UPortalEncapsulatorSphere>(OtherComp);

	if (Sphere != nullptr)
	{
		TeleportCandidateSpheres.Add(Sphere);
		SetActorTickEnabled(true);
	}
}


void ABlackPortal::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	UPortalEncapsulatorBox* Box = Cast<UPortalEncapsulatorBox>(OtherComp);

	if (Box != nullptr)
	{
		TeleportCandidateBoxes.Remove(Box);
	}

	UPortalEncapsulatorSphere* Sphere = Cast<UPortalEncapsulatorSphere>(OtherComp);

	if (Sphere != nullptr)
	{
		TeleportCandidateSpheres.Remove(Sphere);
	}

	if (TeleportCandidateBoxes.Num() == 0 && TeleportCandidateSpheres.Num() == 0) SetActorTickEnabled(false);
}

