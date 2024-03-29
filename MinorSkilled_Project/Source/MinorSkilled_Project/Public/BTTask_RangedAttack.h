// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTTask_RangedAttack.generated.h"

/**
 *
 */
UCLASS()
class MINORSKILLED_PROJECT_API UBTTask_RangedAttack : public UBTTask_BlackboardBase
{
	GENERATED_BODY()

public:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent &pOwnerComp, uint8 *pNodeMemory) override;
};
