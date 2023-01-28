// Fill out your copyright notice in the Description page of Project Settings.


#include "StealthEnemyAICharacter.h"

#include "EnvironmentQuery/EnvQueryManager.h"
#include "GameFramework/CharacterMovementComponent.h"

// Sets default values
AStealthEnemyAICharacter::AStealthEnemyAICharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	// Set this character to orient their movement based on their velocity
	GetCharacterMovement()->bOrientRotationToMovement = true;

	// Create vision cone procedural mesh component
	VisionCone = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("VisionCone"));
	VisionCone->SetupAttachment(RootComponent);
}

void AStealthEnemyAICharacter::BeginPlay()
{
	Super::BeginPlay();

	// If vision cone EQS is assigned and not hidden, start EQS execution timer
	if (!bHideVisionCone && ensureMsgf(VisionConeEQS != nullptr, TEXT("No environment query assigned to actor")))
	{
		GetWorldTimerManager().SetTimer(EQSTimerHandle, this, &AStealthEnemyAICharacter::RunEQS, ConeUpdateFrequency, true);
	}
}

void AStealthEnemyAICharacter::RunEQS()
{
	// Execute vision cone EQS if actor has authority
	if (HasAuthority())
	{
		FEnvQueryRequest SightConeQueryRequest = FEnvQueryRequest(VisionConeEQS, this);
		SightConeQueryRequest.Execute(EEnvQueryRunMode::AllMatching, this, &AStealthEnemyAICharacter::DrawVisionCone);
	}
}

void AStealthEnemyAICharacter::Multicast_SetConeMaterial_Implementation(UMaterialInstance* MaterialInstance)
{
	ConeMaterial = MaterialInstance;
}

void AStealthEnemyAICharacter::Multicast_DrawConeMesh_Implementation(const TArray<FVector>& Vertices,
	const TArray<int>& Triangles)
{
	// Render vision cone procedural mesh
	VisionCone->CreateMeshSection(0, Vertices, Triangles, TArray<FVector>(), TArray<FVector2D>(), TArray<FColor>(), TArray<FProcMeshTangent>(), false);
	VisionCone->SetMaterial(0,ConeMaterial);
	VisionCone->SetWorldRotation(FRotator::ZeroRotator);
}

void AStealthEnemyAICharacter::DrawVisionCone(TSharedPtr<FEnvQueryResult> Result)
{
	// Initialize sight points array and populate with EQS data
	TArray<FVector> SightPoints;
	Result->GetAllAsLocations(SightPoints);

	ConvertSightPointsToVertices(SightPoints);
}

void AStealthEnemyAICharacter::ConvertSightPointsToVertices(const TArray<FVector>& SightPoints)
{
	// Initialize vertices array
	TArray<FVector> Vertices;
	Vertices.SetNum(NumberOfVertices + 1);
	Vertices[0] = FVector::ZeroVector;

	// Populate vertices array using sight points array data
	for (int Index = 0; Index < SightPoints.Num(); Index++)
	{
		const FVector PointPos = FVector(SightPoints[Index].X - GetActorLocation().X, SightPoints[Index].Y - GetActorLocation().Y, -50.f);
		Vertices[Index + 1] = PointPos;
	}

	// Initialize triangles array
	TArray<int> Triangles;
	Triangles.SetNum(Vertices.Num() * 3);

	// Populate triangles array
	for (int Index = 0; Index < Vertices.Num(); Index++)
	{
		Triangles[Index * 3] = 0;
		Triangles[Index * 3 + 1] =  Index + 1;
		Triangles[Index * 3 + 2] = Index + 2;
	}

	// Render mesh with vertices and triangles
	Multicast_DrawConeMesh(Vertices, Triangles);
}

void AStealthEnemyAICharacter::SetSpeedToNormal() const
{
	GetCharacterMovement()->MaxWalkSpeed = BaseSpeed;
}

void AStealthEnemyAICharacter::SetSpeedToChase() const
{
	GetCharacterMovement()->MaxWalkSpeed = ChaseSpeed;
}

