// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_FindNextWaypoint.h"

#include "StealthEnemyAICharacter.h"
#include "StealthEnemyAIController.h"

UBTTask_FindNextWaypoint::UBTTask_FindNextWaypoint(FObjectInitializer const& ObjectInitializer)
{
	NodeName = TEXT("Find next waypoint");
}

EBTNodeResult::Type UBTTask_FindNextWaypoint::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AStealthEnemyAIController* AIController = Cast<AStealthEnemyAIController>(OwnerComp.GetAIOwner());
	checkf(AIController, TEXT("AIController is invalid"));

	AStealthEnemyAICharacter* EnemyPawn = Cast<AStealthEnemyAICharacter>(AIController->GetPawn());
	checkf(EnemyPawn, TEXT("Controller is not possessing a pawn"));

	// Check if waypoint array is empty
	if (const int ArraySize = EnemyPawn->PatrolWaypoints.Num(); ArraySize != 0)
	{
		// If array only has one waypoint, enemy becomes stationary once it reaches it
		if (ArraySize == 1)
			EnemyPawn->SetBehavior(EEnemyBehavior::Stationary);

		// Get waypoint index and behavior
		int PatrolIndex = AIController->GetPatrolIndex();

		if (IsEndOfPathReached(PatrolIndex, ArraySize, AIController->IsReversingPatrol()))
		{
			// Set index according to patrol behavior
			switch (EnemyPawn->GetPatrolMode())
			{
			case EEnemyPatrolMode::Loop: PatrolIndex = 0;
				break;
			case EEnemyPatrolMode::Reverse:
				{
					// When reaching end or beginning of waypoint array, reverse patrol direction
					if(!AIController->IsReversingPatrol())
						PatrolIndex = PatrolIndex - 2 < 0 ? 0 : ArraySize - 2;
					else
						PatrolIndex = PatrolIndex + 2 == ArraySize ? 0 : 1;

					AIController->ToggleReversePatrol();
				} break;
			default:	return EBTNodeResult::Failed;
			}

			AIController->SetPatrolIndex(PatrolIndex);
		}

		checkf(EnemyPawn->PatrolWaypoints[PatrolIndex], TEXT("PatrolIndex is out of PatrolWaypoints array bounds"));

		// Set blackboard target key
		AIController->SetNextWaypointLocation(EnemyPawn->PatrolWaypoints[PatrolIndex]->GetActorLocation());

		// Increment or decrement index depending on array traversal direction
		AIController->IsReversingPatrol() ? PatrolIndex-- : PatrolIndex++;
		AIController->SetPatrolIndex(PatrolIndex);

		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
		return EBTNodeResult::Succeeded;
	}
	// If waypoint array is empty, behavior is set to stationary
	EnemyPawn->SetBehavior(EEnemyBehavior::Stationary);
	return EBTNodeResult::Failed;
}

bool UBTTask_FindNextWaypoint::IsEndOfPathReached(const int PatrolIndex, const int ArraySize, const bool bIsReversingPatrol)
{
	const bool bPathFinished = PatrolIndex == ArraySize || bIsReversingPatrol && PatrolIndex < 0 ?  true : false;
	return bPathFinished;
}
