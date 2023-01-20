// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_ChasePlayer.h"

#include "BlackboardKeys.h"
#include "StealthEnemyAIController.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"

UBTTask_ChasePlayer::UBTTask_ChasePlayer(FObjectInitializer const& ObjectInitializer)
{
	NodeName = TEXT("Chase Player");
}

EBTNodeResult::Type UBTTask_ChasePlayer::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AStealthEnemyAIController* AIController = Cast<AStealthEnemyAIController>(OwnerComp.GetAIOwner());
	checkf(AIController, TEXT("AIController is invalid"));

	if (const AActor* Target = Cast<AActor>(AIController->GetBlackboard()->GetValueAsObject(BBKeys::TargetPlayer)); IsValid(Target))
	{
		UAIBlueprintHelperLibrary::SimpleMoveToLocation(AIController, Target->GetActorLocation());

		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
		return EBTNodeResult::Succeeded;
	}
	return EBTNodeResult::Failed;
}