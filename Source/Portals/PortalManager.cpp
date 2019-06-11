// Fill out your copyright notice in the Description page of Project Settings.


#include "PortalManager.h"
#include "Kismet/GameplayStatics.h"
#include "Camera/PlayerCameraManager.h"
#include "EngineUtils.h" 
#include "WhitePortal.h"
#include "BlackPortal.h"
#include "PortalPlayerController.h"

// Sets default values
APortalManager::APortalManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	this->SetTickGroup(ETickingGroup::TG_PostUpdateWork);

}

// Called when the game starts or when spawned
void APortalManager::BeginPlay()
{
	Super::BeginPlay();

	
	for (TActorIterator<AWhitePortal> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		WhitePortals.Add(*ActorItr);
	}
	for (TActorIterator<ABlackPortal> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		BlackPortals.Add(*ActorItr);
	}

}

// Called every frame
void APortalManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	APlayerCameraManager* CameraManager = UGameplayStatics::GetPlayerCameraManager(this, 0);


	//white portal update stuff
	USceneComponent* CameraTransform = CameraManager->GetTransformComponent();

	UpdatePortalRelevancy();
	
	for (AWhitePortal* CurrentPortal : RelevantWhitePortals) //questo deve diventare un for loop da 1 ad n che termina se ho finito l'array o se n>max portals
	{
		CurrentPortal->UpdatePortalRender(CameraTransform->GetComponentLocation(), CameraTransform->GetComponentRotation());
		//UE_LOG(LogTemp, Log, TEXT("Rendering portal %s"), *CurrentPortal->GetName());
	}

}

void APortalManager::UpdatePortalRelevancy()
{
	APlayerCameraManager* CameraManager = UGameplayStatics::GetPlayerCameraManager(this, 0);

	//black portal update stuff
	APortalPlayerController* Controller = Cast<APortalPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	
	RelevantBlackPortals.Empty();
	RelevantWhitePortals.Empty();

	for (ABlackPortal* CurrentPortal : BlackPortals)
	{
		CurrentPortal->CurrentScreenSize = Controller->GetPortalScreenRadius(CurrentPortal, CameraManager);

		if (CurrentPortal->CurrentScreenSize > RelevantPortalScreenSize) AddRelevantPortal(CurrentPortal);
	}
}

void APortalManager::AddRelevantPortal(ABlackPortal * PortalToAdd)
{
	for (int i = 0; i<RelevantBlackPortals.Num(); i++)
	{
		if (RelevantBlackPortals[i]->CurrentScreenSize < PortalToAdd->CurrentScreenSize)
		{
			RelevantBlackPortals.Insert(PortalToAdd, i);
			RelevantWhitePortals.Insert(PortalToAdd->WhitePortal, i);
			return;
		}
	}

	RelevantBlackPortals.Add(PortalToAdd); //if the array is empty OR we didn't find any place to insert we just add it
	RelevantWhitePortals.Add(PortalToAdd->WhitePortal);

	return;

}

