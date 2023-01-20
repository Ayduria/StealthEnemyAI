// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "EnemyStates.h"
#include "Engine/TargetPoint.h"
#include "ProceduralMeshComponent.h"
#include "StealthEnemyAICharacter.generated.h"

/**
 * Base for an AI enemy in a game with stealth elements
 * Behaviors include patrolling, investigating, chasing and returning to assigned position
 */
UCLASS()
class STEALTHENEMYAI_API AStealthEnemyAICharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AStealthEnemyAICharacter();

protected:
	/** The current state of the enemy */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Behavior")
	EEnemyState State;

	/** The behavior of the enemy */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Behavior")
	EEnemyBehavior Behavior;

	/** The behavior of a patrolling enemy once it reaches its last waypoint */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Behavior|Patrol", meta=(EditConditionHides = true, EditCondition = "Behavior == EEnemyBehavior::Patrol"))
	EEnemyPatrolMode PatrolMode;

	/** If true, enemy will not stop at each waypoint */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Behavior|Patrol", meta=(EditConditionHides = true, EditCondition = "Behavior == EEnemyBehavior::Patrol"))
	bool bSmoothPatrol = false;

	/** The base speed of the enemy */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Behavior", meta=(EditConditionHides = true, EditCondition = "Behavior == EEnemyBehavior::Patrol"))
	float BaseSpeed = 300.f;

	/** The investigate speed of the enemy */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Behavior")
	float InvestigateSpeed = 450.f;

	/** The chase speed of the enemy */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Behavior")
	float ChaseSpeed = 600.f;

	UFUNCTION(BlueprintImplementableEvent)
	void SetConeMaterial(UMaterialInstance* MaterialInstance);

public:
	/** The behavior tree that dictates the enemy's behavior */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Behavior")
	class UBehaviorTree* BehaviorTree;

	/** The array that stores the waypoints for the patrol */
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category="Behavior|Patrol", meta=(EditConditionHides = true, EditCondition = "Behavior == EEnemyBehavior::Patrol"))
	TArray<ATargetPoint*> PatrolWaypoints;

	// Getters for enums
	EEnemyBehavior GetBehavior() const;
	EEnemyPatrolMode GetPatrolMode() const;
	EEnemyState GetState() const;

	// Setters for enums
	void SetBehavior(EEnemyBehavior EnemyBehavior);
	void SetPatrolMode(EEnemyPatrolMode EnemyPatrolMode);
	void SetState(EEnemyState EnemyState);

	UFUNCTION(BlueprintCallable)
	void SetSpeedToNormal() const;

	UFUNCTION(BlueprintCallable)
	void SetSpeedToInvestigate() const;

	UFUNCTION(BlueprintCallable)
	void SetSpeedToChase() const;

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_SetConeMaterial(UMaterialInstance* MaterialInstance);

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UProceduralMeshComponent* VisionCone;
};
