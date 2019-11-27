// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyAIController.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "EnemyPaperCharacter.h"
#include "BehaviorTree/BlackboardData.h"
#include "BehaviorTree/BehaviorTree.h"
#include "UserWidget.h"

AEnemyAIController::AEnemyAIController()
{
	BlackboardComponent = CreateDefaultSubobject<UBlackboardComponent>("Blackboard Component");
	BehaviourTreeComponent = CreateDefaultSubobject<UBehaviorTreeComponent>("BehaviourTree Component");
}

void AEnemyAIController::OnPossess(APawn *pPawn)
{
	Super::OnPossess(pPawn);

	EnemyCharacter = Cast<AEnemyPaperCharacter>(pPawn);

	if(EnemyCharacter && EnemyCharacter->BehaviourTree)
	{
		BlackboardComponent->InitializeBlackboard(*EnemyCharacter->BehaviourTree->BlackboardAsset);

		TargetKeyID = BlackboardComponent->GetKeyID("Target");

		BehaviourTreeComponent->StartTree(*EnemyCharacter->BehaviourTree);
	}
}
