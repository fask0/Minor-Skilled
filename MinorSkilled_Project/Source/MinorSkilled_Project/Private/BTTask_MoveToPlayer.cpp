// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_MoveToPlayer.h"
#include "EnemyAIController.h"
#include "PlayerPaperCharacter.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyAllTypes.h"
#include "Engine/Engine.h"
#include "EnemyPaperCharacter.h"
#include "Navigation/NavLinkProxy.h"

EBTNodeResult::Type UBTTask_MoveToPlayer::ExecuteTask(UBehaviorTreeComponent &pOwnerComp, uint8 *pNodeMemory)
{
	AEnemyAIController *enemyController = Cast<AEnemyAIController>(pOwnerComp.GetAIOwner());

	APlayerPaperCharacter *player = Cast<APlayerPaperCharacter>(pOwnerComp.GetBlackboardComponent()->GetValue<UBlackboardKeyType_Object>(enemyController->TargetKeyID));

	if(player)
	{
		if(enemyController->EnemyCharacter->ShouldWait)
		{
			FVector dist = player->GetActorLocation() - enemyController->EnemyCharacter->GetActorLocation();
			float dir = FVector::DotProduct(dist, enemyController->EnemyCharacter->GetActorForwardVector());
			if(dir < 0)
				enemyController->SetControlRotation(FRotator(0, enemyController->EnemyCharacter->GetActorRotation().Yaw - 180, 0));
			return EBTNodeResult::Succeeded;
		}

		if(enemyController->EnemyCharacter->IsInAir)
		{
			enemyController->MoveToActor(player, AcceptanceRadius, true, false, false, 0, false);
		}
		else
		{
			const float zDifference = player->GetActorLocation().Z - enemyController->EnemyCharacter->GetActorLocation().Z;
			if(player->GetVelocity().Z != 0 && (zDifference > 0 && zDifference < AcceptanceRadius * 3))
				enemyController->MoveToLocation(FVector(player->GetActorLocation().X, player->GetActorLocation().Y, enemyController->EnemyCharacter->GetActorLocation().Z), AcceptanceRadius, true, true, false, 0, false);
			else if(!enemyController->MoveToActor(player, AcceptanceRadius, true, true, false, 0, false))
				enemyController->MoveToActor(player, AcceptanceRadius, true, false, false, 0, false);
		}
		return EBTNodeResult::Succeeded;
	}
	else
	{
		return EBTNodeResult::Failed;
	}

	return EBTNodeResult::Failed;
}
