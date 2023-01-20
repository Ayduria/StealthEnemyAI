// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_FindTargetLocation.h"

#include "BlackboardKeys.h"
#include "StealthEnemyAIController.h"

UBTTask_FindTargetLocation::UBTTask_FindTargetLocation(FObjectInitializer const& ObjectInitializer)
{
	NodeName = TEXT("Find Target Location");
}

EBTNodeResult::Type UBTTask_FindTargetLocation::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	const AStealthEnemyAIController* AIController = Cast<AStealthEnemyAIController>(OwnerComp.GetAIOwner());
	checkf(AIController, TEXT("AIController is invalid"));

	// Get target actor from blackboard
	if (const AActor* Target = Cast<AActor>(AIController->GetBlackboard()->GetValueAsObject(BBKeys::TargetPlayer)); IsValid(Target))
	{
		// Set target position in blackboard
		AIController->SetTargetLocation(Target->GetActorLocation());

		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
		return EBTNodeResult::Succeeded;
	}
	return EBTNodeResult::Failed;
}
