// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "EnemyStates.h"
#include "Engine/TargetPoint.h"
#include "ProceduralMeshComponent.h"
#include "EnvironmentQuery/EnvQuery.h"
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
	virtual void BeginPlay() override;

	/** The behavior tree that dictates the enemy's behavior */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AI")
	class UBehaviorTree* BehaviorTree;

	/** The array that stores the waypoints for the patrol */
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category="AI|Patrol", meta=(EditConditionHides = true, EditCondition = "Behavior == EEnemyBehavior::Patrol"))
	TArray<ATargetPoint*> PatrolWaypoints;

	// Getters
	EEnemyBehavior GetBehavior() const { return Behavior; }
	EEnemyPatrolMode GetPatrolMode() const { return PatrolMode; }
	EEnemyState GetState() const { return State; }
	bool IsPatrolSmooth() const { return bSmoothPatrol; }

	// Setters
	void SetBehavior(const EEnemyBehavior EnemyBehavior) { Behavior = EnemyBehavior; }
	void SetPatrolMode(const EEnemyPatrolMode EnemyPatrolMode) { PatrolMode = EnemyPatrolMode; }
	void SetState(const EEnemyState EnemyState) { State = EnemyState; }

	/** Sets enemy speed to base speed */
	UFUNCTION(BlueprintCallable)
	void SetSpeedToNormal() const;

	/** Sets enemy speed to chase speed */
	UFUNCTION(BlueprintCallable)
	void SetSpeedToChase() const;

	/** Sets vision cone material
	 * Used to change vision cone color */
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_SetConeMaterial(UMaterialInstance* MaterialInstance);

protected:
	/** The current state of the enemy */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category = "AI", meta=(EditConditionHides = true, EditCondition = "BehaviorTree != nullptr"))
	EEnemyState State;

	/** The behavior of the enemy */
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "AI", meta=(EditConditionHides = true, EditCondition = "BehaviorTree != nullptr"))
	EEnemyBehavior Behavior;

	/** The behavior of a patrolling enemy once it reaches its last waypoint */
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "AI|Patrol", meta=(EditConditionHides = true, EditCondition = "Behavior == EEnemyBehavior::Patrol"))
	EEnemyPatrolMode PatrolMode;

	/** If true, enemy will not stop at each waypoint
	 * It will still stop at the end of waypoint array in back and forth patrol mode */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AI|Patrol", meta=(EditConditionHides = true, EditCondition = "Behavior == EEnemyBehavior::Patrol"))
	bool bSmoothPatrol = false;

	/** The base speed of the enemy */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AI", meta=(EditConditionHides = true, EditCondition = "BehaviorTree != nullptr"))
	float BaseSpeed = 300.f;

	/** The chase speed of the enemy */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI", meta=(EditConditionHides = true, EditCondition = "BehaviorTree != nullptr"))
	float ChaseSpeed = 600.f;

	/** The vision cone procedural mesh component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UProceduralMeshComponent* VisionCone;

	/** The default material for the vision cone procedural mesh component */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	UMaterialInstance* ConeMaterial;

	/** The EQS for the vision cone mesh generation */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AI|Vision Cone", meta = (AllowPrivateAccess = "true"))
	UEnvQuery* VisionConeEQS;

	/** If this box is ticked, this instance of an enemy will not display a vision cone */
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "AI|Vision Cone", meta = (AllowPrivateAccess = "true"), meta=(EditConditionHides = true, EditCondition = "VisionConeEQS != nullptr"))
	bool bHideVisionCone = false;

	/* For optimal functioning, set this number to the number of points in EQS Query */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AI|Vision Cone", meta = (AllowPrivateAccess = "true"), meta=(EditConditionHides = true, EditCondition = "VisionConeEQS != nullptr"))
	int NumberOfVertices = 25;

	/** The interval of time at which the vision cone mesh is updated */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AI|Vision Cone", meta = (AllowPrivateAccess = "true"), meta=(EditConditionHides = true, EditCondition = "VisionConeEQS != nullptr"))
	float ConeUpdateFrequency = 0.1f;

	// The timer handle for the EQS calls
	FTimerHandle EQSTimerHandle;

	// Called when EQS result is received
	// Fills arrays for vision cone mesh data
	void DrawVisionCone(TSharedPtr<FEnvQueryResult> Result);

	// Converts EQS points to vertices for vision cone mesh
	void ConvertSightPointsToVertices(const TArray<FVector>& SightPoints);

	/** Executes EQS for vision cone
	 * Called by timer */
	UFUNCTION()
	void RunEQS();

	/** Renders the vision cone procedural mesh */
	UFUNCTION(NetMulticast, Unreliable)
	void Multicast_DrawConeMesh(const TArray<FVector>& Vertices, const TArray<int>& Triangles);
};
