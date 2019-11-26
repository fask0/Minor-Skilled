// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_MeleeAttack.h"
#include "EnemyAIController.h"
#include "PlayerPaperCharacter.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyAllTypes.h"
#include "Engine/Engine.h"
#include "EnemyPaperCharacter.h"
#include "Navigation/NavLinkProxy.h"
#include "PaperCharacter.h"
#include "PaperFlipbookComponent.h"

EBTNodeResult::Type UBTTask_MeleeAttack::ExecuteTask(UBehaviorTreeComponent &pOwnerComp, uint8 *pNodeMemory)
{
	AEnemyAIController *enemyController = Cast<AEnemyAIController>(pOwnerComp.GetAIOwner());

	enemyController->EnemyCharacter->Attack();

	return EBTNodeResult::Failed;
}
