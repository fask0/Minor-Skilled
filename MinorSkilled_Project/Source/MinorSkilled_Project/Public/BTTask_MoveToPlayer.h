// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTTask_MoveToPlayer.generated.h"

/**
 *
 */
UCLASS()
class MINORSKILLED_PROJECT_API UBTTask_MoveToPlayer : public UBTTask_BlackboardBase
{
	GENERATED_BODY()

public:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent &pOwnerComp, uint8 *pNodeMemory) override;

protected:
	UPROPERTY(EditAnywhere, Category = Movement)
		float AcceptanceRadius;
	UPROPERTY(EditAnywhere, Category = Movement)
		float MeleeAttackRange;
};
