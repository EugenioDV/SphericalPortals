// Fill out your copyright notice in the Description page of Project Settings.


#include "PortalManager.h"
#include "Kismet/GameplayStatics.h"
#include "Camera/PlayerCameraManager.h"
#include "EngineUtils.h" 
#include "WhitePortal.h"
#include "BlackPortal.h"
#include "PortalPlayerController.h"
#include "Engine/StaticMeshActor.h" //temporarly needed for our portal candidates list thing, we should know better.
#include "Kismet/KismetRenderingLibrary.h" 

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
		ActorItr->UpdateRenderCandidates(&CandidatesForPortalRender);
	}

	for (TActorIterator<ABlackPortal> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		BlackPortals.Add(*ActorItr);
	}
	
	PortalController = Cast<APortalPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	CameraManager = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0);

}

// Called every frame
void APortalManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	BuildPortalRenderCandidatesList(); //this shouldn't be done every frame, but will do for now

	DynamicPortalUpdate(); //adjusts fov and resolution dynamically

	UpdatePortalRelevancy();

	USceneComponent* CameraTransform = CameraManager->GetTransformComponent();
	int Max = FMath::Min(RelevantWhitePortals.Num(), (int)MaxActivePortals);

	for (int i = 0; i<Max; ++i)
	{
		RelevantWhitePortals[i]->UpdatePortalRender(CameraTransform->GetComponentLocation(), CameraTransform->GetComponentRotation(), RenderTargets[i]);
		RelevantBlackPortals[i]->UpdateRenderTarget(RenderTargets[i]);
	}
	
}

void APortalManager::UpdateFovAngle(float NewFov)
{
	for (AWhitePortal* CurrentWhitePortal : WhitePortals) {
		CurrentWhitePortal->UpdateRenderTargetFOV(NewFov);
	}
}

void APortalManager::UpdateRenderTargetTextures(int NewResX, int NewResY)
{
	//destroy previous render targets?
	for (int i = 0; i < MaxActivePortals; ++i)
	{
		RenderTargets[i] = UKismetRenderingLibrary::CreateRenderTarget2D(GetWorld(), NewResX, NewResY);
	}
}

void APortalManager::UpdatePortalRelevancy()
{

	if (PortalController == nullptr) {
		UE_LOG(LogTemp, Error, TEXT("Error! PortalController not found. Is your Player Controller a PortalPlayerController or derived class?"));
		return;
	}

	RelevantBlackPortals.Empty();
	RelevantWhitePortals.Empty();

	for (ABlackPortal* CurrentPortal : BlackPortals)
	{
		CurrentPortal->CurrentScreenSize = PortalController->GetPortalScreenSize(CurrentPortal, CameraManager);

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

void APortalManager::DynamicPortalUpdate()
{
	if (PortalController != nullptr) {
		if (PortalController->GetCurrentGameResolution(CurrentResX, CurrentResY)) {
			if (CurrentResX != StoredResX || CurrentResY != StoredResY) {
				StoredResX = CurrentResX;
				StoredResY = CurrentResY;
				UpdateRenderTargetTextures(CurrentResX, CurrentResY);
			}
		}
	}

	if (CameraManager != nullptr) {
		if (CameraManager->GetFOVAngle() != StoredFovAngle) {
			StoredFovAngle = CameraManager->GetFOVAngle();
				UpdateFovAngle(StoredFovAngle);
		}
	}

}

void APortalManager::BuildPortalRenderCandidatesList()
{
	CandidatesForPortalRender.Empty();

	for (TActorIterator<AActor> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		if (ActorItr->ActorHasTag(TEXT("RenderedInPortal")))	CandidatesForPortalRender.Add(*ActorItr);
	}

}

