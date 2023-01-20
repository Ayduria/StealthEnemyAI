// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTTask_FindNextWaypoint.generated.h"

/**
 * 
 */
UCLASS()
class STEALTHENEMYAI_API UBTTask_FindNextWaypoint : public UBTTask_BlackboardBase
{
	GENERATED_BODY()

public:
	UBTTask_FindNextWaypoint(FObjectInitializer const& ObjectInitializer);

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	static bool IsEndOfPathReached(int PatrolIndex, int ArraySize, bool bIsReversingPatrol);
	
};
