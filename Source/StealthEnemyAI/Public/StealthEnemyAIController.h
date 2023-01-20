// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "EnemyStates.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Perception/AIPerceptionTypes.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AISenseConfig_Hearing.h"
#include "StealthEnemyAIController.generated.h"

/**
 * 
 */
UCLASS()
class STEALTHENEMYAI_API AStealthEnemyAIController : public AAIController
{
	GENERATED_BODY()

public:
	AStealthEnemyAIController();

	virtual void BeginPlay() override;
	virtual void OnPossess(APawn* InPawn) override;;
	UBlackboardComponent* GetBlackboard() const;

	// Getter and setter for patrol waypoints index
	int GetPatrolIndex() const;
	void SetPatrolIndex(int Index);

	// Getter and setter for patrol direction boolean
	bool IsReversingPatrol() const;
	void ToggleReversePatrol();

	// Getters for stationary enemy base position and rotation
	UFUNCTION(BlueprintCallable)
	FVector GetBasePosition() const;
	UFUNCTION(BlueprintCallable)
	FRotator GetBaseRotation() const;

	/** Utility function to reset a stationary enemy's rotation
	 * Used to face correct direction when resuming guarding duty */
	UFUNCTION(BlueprintCallable)
	void FaceCorrectDirection(FRotator Rotation);

	/** Returns true if enemy has Stationary behavior */
	UFUNCTION(BlueprintCallable)
	bool IsEnemyStationary() const;

	/** Returns true if enemy has Suspicious state */
	UFUNCTION(BlueprintCallable)
	bool IsSuspicious() const;

	/** Returns true if enemy has Investigating state */
	UFUNCTION(BlueprintCallable)
	bool IsInvestigating() const;

	/** Sets the enemy state in enum and blackboard */
	UFUNCTION(BlueprintCallable)
	void SetState(EEnemyState State) const;

	/** Sets target actor in blackboard */
	UFUNCTION(BlueprintCallable)
	void SetTargetActor(AActor* Target) const;

	/** Sets target player in blackboard */
	UFUNCTION(BlueprintCallable)
	void SetTargetPlayer(ACharacter* Player) const;

	/** Sets target location in blackboard */
	UFUNCTION(BlueprintCallable)
	void SetTargetLocation(FVector Location) const;

	/** Sets next waypoint location in blackboard */
	UFUNCTION(BlueprintCallable)
	void SetNextWaypointLocation(FVector Location) const;

private:
	/** The component that starts the behavior tree */
	UPROPERTY(Transient)
	UBehaviorTreeComponent* BehaviorTreeComponent;

	/** The component that initializes the blackboard */
	UPROPERTY(Transient)
	UBlackboardComponent* BlackboardComponent;

	/** Sight config for player detection */
	UPROPERTY()
	class UAISenseConfig_Sight* SightConfig;

	/** Hearing config for player detection */
	UPROPERTY()
	class UAISenseConfig_Hearing* HearingConfig;

	// Sense IDs for sense type detection
	FAISenseID SightID;
	FAISenseID HearingID;

	// The index of the patrol target waypoint. Set by AI task
	int WaypointIndex = 0;

	// Boolean switch that determines array traversal direction. Set by AI task
	bool bReversingPatrol = false;

	// Called when AI detects actor through a sense
	UFUNCTION()
	void OnTargetDetected(AActor* Actor, FAIStimulus const Stimulus);

	// Called when AI has heard a noise through OnTargetDetected
	void HeardNoise(FAIStimulus Stimulus);

	// Initializes SightConfig parameters
	void SetupPerceptionSystem();

	/** The enemy pawn possessed by this controller */
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	AStealthEnemyAICharacter* ControlledPawn;

	/** Delay between enemy getting suspicious and starting investigation */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"), Category = "Investigation")
	float DelayBeforeInvestigating = 2.0f;

	/** Duration of enemy investigation */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"), Category = "Investigation")
	float InvestigatingDuration = 10.0f;

	// Base state of enemy, changes based on behavior
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	EEnemyState BaseState = EEnemyState::Guarding;

	// Base position and rotation of stationary enemy
	FVector BasePosition;
	FRotator BaseRotation;

	// Timer handle for investigation
	FTimerHandle InvestigateTimerHandle;

	// Materials for vision cone
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly,  Category = "Vision Cone", meta = (AllowPrivateAccess = "true"))
	UMaterialInstance* DefaultConeMaterial;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly,  Category = "Vision Cone", meta = (AllowPrivateAccess = "true"))
	UMaterialInstance* SusConeMaterial;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly,  Category = "Vision Cone", meta = (AllowPrivateAccess = "true"))
	UMaterialInstance* ChaseConeMaterial;

	/** Sets enemy in Suspicious state
	 * Called when enemy hears noise */
	UFUNCTION()
	void SetSuspicious(FName NoiseTag);

	/** Sets enemy in Investigating state
	 * Called after suspicion timer */
	UFUNCTION()
	void StartInvestigating();

	/** Returns enemy to default state if investigation is not conclusive */
	UFUNCTION()
	void ResumeActivities();

	/** Sets enemy in Chasing state
	 * Called when enemy sees player or drone */
	UFUNCTION()
	void StartChase(ACharacter* Player);

	/** Returns next waypoint location if enemy has Patrol behavior
	 * Will return ZeroVector if enemy has Stationary behavior */
	UFUNCTION(BlueprintCallable)
	FVector GetNextWaypointLocation() const;
	
};
