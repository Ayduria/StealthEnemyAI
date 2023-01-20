// Fill out your copyright notice in the Description page of Project Settings.


#include "StealthEnemyAICharacter.h"

#include "GameFramework/CharacterMovementComponent.h"

// Sets default values
AStealthEnemyAICharacter::AStealthEnemyAICharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	GetCharacterMovement()->bOrientRotationToMovement = true;

	VisionCone = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("VisionCone"));
	VisionCone->SetupAttachment(RootComponent);
}

EEnemyBehavior AStealthEnemyAICharacter::GetBehavior() const
{
	return Behavior;
}

void AStealthEnemyAICharacter::SetBehavior(const EEnemyBehavior EnemyBehavior)
{
	Behavior = EnemyBehavior;
}

EEnemyPatrolMode AStealthEnemyAICharacter::GetPatrolMode() const
{
	return PatrolMode;
}

void AStealthEnemyAICharacter::SetPatrolMode(const EEnemyPatrolMode EnemyPatrolMode)
{
	PatrolMode = EnemyPatrolMode;
}

EEnemyState AStealthEnemyAICharacter::GetState() const
{
	return State;
}

void AStealthEnemyAICharacter::SetState(const EEnemyState EnemyState)
{
	State = EnemyState;
}

void AStealthEnemyAICharacter::SetSpeedToNormal() const
{
	GetCharacterMovement()->MaxWalkSpeed = BaseSpeed;
}

void AStealthEnemyAICharacter::SetSpeedToInvestigate() const
{
	GetCharacterMovement()->MaxWalkSpeed = InvestigateSpeed;
}

void AStealthEnemyAICharacter::SetSpeedToChase() const
{
	GetCharacterMovement()->MaxWalkSpeed = ChaseSpeed;
}

void AStealthEnemyAICharacter::Multicast_SetConeMaterial_Implementation(UMaterialInstance* MaterialInstance)
{
	SetConeMaterial(MaterialInstance);
}

