// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTTask_ResumeBaseDuty.generated.h"

/**
 * 
 */
UCLASS()
class STEALTHENEMYAI_API UBTTask_ResumeBaseDuty : public UBTTask_BlackboardBase
{
	GENERATED_BODY()

	UBTTask_ResumeBaseDuty(FObjectInitializer const& ObjectInitializer);

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	
};
