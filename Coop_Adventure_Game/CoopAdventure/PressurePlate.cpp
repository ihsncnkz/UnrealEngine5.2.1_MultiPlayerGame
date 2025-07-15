// Fill out your copyright notice in the Description page of Project Settings.


#include "PressurePlate.h"

APressurePlate::APressurePlate()
{
	PrimaryActorTick.bCanEverTick = true;

	bReplicates = true;
	SetReplicateMovement(true);

	Activated = false;

	RootComp = CreateDefaultSubobject<USceneComponent>(TEXT("RootComp"));
	SetRootComponent(RootComp);

	TriggerMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TriggerMesh"));
	TriggerMesh->SetupAttachment(RootComp);
	TriggerMesh->SetIsReplicated(true);

	auto TriggerMeshAsset = ConstructorHelpers::FObjectFinder<UStaticMesh>(TEXT("/Game/StarterContent/Shapes/Shape_Cylinder.Shape_Cylinder"));
	if (TriggerMeshAsset.Succeeded())
	{
		TriggerMesh->SetStaticMesh(TriggerMeshAsset.Object);
		TriggerMesh->SetRelativeScale3D(FVector(3.3f, 3.3f, 0.2f));
		TriggerMesh->SetRelativeLocation(FVector(0.0f, 0.0f, 10.0f));
	}

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(RootComp);
	Mesh->SetIsReplicated(true);

	auto MeshAsset = ConstructorHelpers::FObjectFinder<UStaticMesh>(TEXT("/Game/Stylized_Egypt/Meshes/building/SM_building_part_08.SM_building_part_08"));
	if (MeshAsset.Succeeded())
	{
		Mesh->SetStaticMesh(MeshAsset.Object);
		Mesh->SetRelativeScale3D(FVector(4.0f, 4.0f, 0.5f));
		Mesh->SetRelativeLocation(FVector(0.0f, 0.0f, 7.2f));
	}

	Transporter = CreateDefaultSubobject<UTransporter>(TEXT("Transporter"));
	Transporter->MoveTime = 0.25f;
	Transporter->OwnerIsTriggerActor = true;
}	

void APressurePlate::BeginPlay()
{
	Super::BeginPlay();

	TriggerMesh->SetVisibility(false);
	TriggerMesh->SetCollisionProfileName(FName("OverlapAll"));

	FVector Point1 = GetActorLocation();
	FVector Point2 = Point1 + FVector(0.f, 0.f, -10.f);
	Transporter->SetPoints(Point1, Point2);
}

void APressurePlate::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (HasAuthority())
	{
		TArray<AActor*> OverlappingActors;
		AActor* TriggerActor = 0;
		TriggerMesh->GetOverlappingActors(OverlappingActors);

		for (int ActorIdx = 0; ActorIdx < OverlappingActors.Num(); ++ActorIdx)
		{
			AActor* A = OverlappingActors[ActorIdx];
			if (A->ActorHasTag("TriggerActor"))
			{
				TriggerActor = A;
				break;
			}
			//FString Msg = FString::Printf(TEXT("Name: %s"), *A->GetName());
			//GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Cyan, Msg);
		}

		if (TriggerActor)
		{
			if (!Activated)
			{
				Activated = true;
				//GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Cyan, TEXT("Activated"));
				OnActivated.Broadcast();
			}
		}
		else
		{
			if (Activated)
			{
				Activated = false;
				//GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Cyan, TEXT("Deactivated"));
				OnDeactivated.Broadcast();
			}
		}
	}
}


