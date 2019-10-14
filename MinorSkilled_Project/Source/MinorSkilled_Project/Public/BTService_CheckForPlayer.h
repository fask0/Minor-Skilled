// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTService_CheckForPlayer.generated.h"

/**
 *
 */
UCLASS()
class MINORSKILLED_PROJECT_API UBTService_CheckForPlayer : public UBTService
{
	GENERATED_BODY()

public:
	UBTService_CheckForPlayer();

	virtual void TickNode(UBehaviorTreeComponent &pOwnerComp, uint8 *pNodeMemory, float pDeltaSeconds) override;

protected:

};
