// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_ResumeBaseDuty.h"

#include "StealthEnemyAIController.h"

UBTTask_ResumeBaseDuty::UBTTask_ResumeBaseDuty(FObjectInitializer const& ObjectInitializer)
{
	NodeName = TEXT("Resume base duty");
}

EBTNodeResult::Type UBTTask_ResumeBaseDuty::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AStealthEnemyAIController* AIController = Cast<AStealthEnemyAIController>(OwnerComp.GetAIOwner());
	checkf(AIController, TEXT("AIController is invalid"));

	const FRotator Rotation = AIController->GetBaseRotation();

	// If enemy has Stationary behavior, go back to guarding
	// Otherwise, resume patrolling
	if (AIController->IsEnemyStationary())
	{
		AIController->FaceCorrectDirection(Rotation);
		AIController->SetState(EEnemyState::Guarding);
	} else
		AIController->SetState(EEnemyState::Patrolling);

	FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	return EBTNodeResult::Succeeded;
}
