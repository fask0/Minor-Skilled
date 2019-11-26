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
		const FVector playerLocation = player->GetActorLocation();
		const FVector enemyLocation = enemyController->EnemyCharacter->GetActorLocation();
		const float distanceToPlayer = FVector::Dist(enemyLocation, playerLocation);
		enemyController->EnemyCharacter->CurrTargetLocation = playerLocation;

		if(enemyController->EnemyCharacter->ShouldWait)
		{
			FVector dist = playerLocation - enemyLocation;
			float dir = FVector::DotProduct(dist, enemyController->EnemyCharacter->GetActorForwardVector());
			if(dir < 0)
				enemyController->SetControlRotation(FRotator(0, enemyController->EnemyCharacter->GetActorRotation().Yaw - 180, 0));
		}

		if(distanceToPlayer <= AcceptanceRadius * 2)
		{
			return EBTNodeResult::Failed;
		}

		if(enemyController->EnemyCharacter->IsInAir)
		{
			enemyController->MoveToActor(player, AcceptanceRadius, true, false, false, 0, false);
		}
		else
		{
			const float zDifference = playerLocation.Z - enemyLocation.Z;
			if(player->GetVelocity().Z != 0 && (zDifference > 0 && zDifference < AcceptanceRadius * 4))
				enemyController->MoveToLocation(FVector(playerLocation.X, playerLocation.Y, enemyLocation.Z), AcceptanceRadius, true, true, false, 0, false);
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
