// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "EnemyAIController.generated.h"

/**
 *
 */
UCLASS()
class MINORSKILLED_PROJECT_API AEnemyAIController : public AAIController
{
	GENERATED_BODY()

public:
	AEnemyAIController();

	virtual void OnPossess(APawn *pPawn) override;

	class AEnemyPaperCharacter *EnemyCharacter;
	uint8 TargetKeyID;
	uint8 TargetLocationKeyID;

protected:
	UPROPERTY(Transient)
		class UBlackboardComponent *BlackboardComponent;

	UPROPERTY(Transient)
		class UBehaviorTreeComponent *BehaviourTreeComponent;
};
