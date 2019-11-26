// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_MoveAwayFromPlayer.h"
#include "EnemyAIController.h"
#include "PlayerPaperCharacter.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyAllTypes.h"
#include "Engine/Engine.h"
#include "EnemyPaperCharacter.h"
#include "Navigation/NavLinkProxy.h"
#include "Components/CapsuleComponent.h"

EBTNodeResult::Type UBTTask_MoveAwayFromPlayer::ExecuteTask(UBehaviorTreeComponent &pOwnerComp, uint8 *pNodeMemory)
{
	AEnemyAIController *enemyController = Cast<AEnemyAIController>(pOwnerComp.GetAIOwner());

	APlayerPaperCharacter *player = Cast<APlayerPaperCharacter>(pOwnerComp.GetBlackboardComponent()->GetValue<UBlackboardKeyType_Object>(enemyController->TargetKeyID));

	if(player)
	{
		const FVector playerLocation = player->GetActorLocation();
		const FVector enemyLocation = enemyController->EnemyCharacter->GetActorLocation();
		const float distanceToPlayer = FVector::Dist(enemyLocation, playerLocation);

		//Follow Player
		if(distanceToPlayer > AcceptanceRadius)
		{
			enemyController->EnemyCharacter->CurrTargetLocation = playerLocation;
			if(enemyController->EnemyCharacter->ShouldWait)
			{
				FVector dist = playerLocation - enemyLocation;
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
				const float zDifference = playerLocation.Z - enemyLocation.Z;
				if(player->GetVelocity().Z != 0 && (zDifference > 0 && zDifference < AcceptanceRadius * 3))
					enemyController->MoveToLocation(FVector(playerLocation.X, playerLocation.Y, enemyLocation.Z), AcceptanceRadius, true, true, false, 0, false);
				else if(!enemyController->MoveToActor(player, AcceptanceRadius, true, true, false, 0, false))
					enemyController->MoveToActor(player, AcceptanceRadius, true, false, false, 0, false);
			}
		}
		//Chose a new location away from Player
		else if(distanceToPlayer < AcceptanceRadius * 0.2f)
		{
			if(hasMovedAwayFromPlayer)
			{
				enemyController->EnemyCharacter->GetRandomLocationInNavArea(AcceptanceRadius);
				enemyController->EnemyCharacter->RandomLocationOnNavArea.Y = 0;
				float newDistanceToPlayer = FVector::Dist(enemyController->EnemyCharacter->RandomLocationOnNavArea, player->GetActorLocation());
				while(newDistanceToPlayer < distanceToPlayer)
				{
					enemyController->EnemyCharacter->GetRandomLocationInNavArea(AcceptanceRadius);
					enemyController->EnemyCharacter->RandomLocationOnNavArea.Y = 0;
					newDistanceToPlayer = FVector::Dist(enemyController->EnemyCharacter->RandomLocationOnNavArea, player->GetActorLocation());
				}
				hasMovedAwayFromPlayer = false;
				enemyController->EnemyCharacter->GetCapsuleComponent()->SetCollisionProfileName(TEXT("OverlapOnlyPawn"));
			}
		}

		if(!hasMovedAwayFromPlayer)
		{
			enemyController->EnemyCharacter->CurrTargetLocation = enemyController->EnemyCharacter->RandomLocationOnNavArea;
			if(enemyController->EnemyCharacter->ShouldWait)
			{
				FVector dist = enemyController->EnemyCharacter->RandomLocationOnNavArea - enemyLocation;
				float dir = FVector::DotProduct(dist, enemyController->EnemyCharacter->GetActorForwardVector());
				if(dir < 0)
					enemyController->SetControlRotation(FRotator(0, enemyController->EnemyCharacter->GetActorRotation().Yaw - 180, 0));
				return EBTNodeResult::Succeeded;
			}
			else
			{
				if(!enemyController->MoveToLocation(enemyController->EnemyCharacter->RandomLocationOnNavArea, 10, true, true, false, 0, false))
					enemyController->MoveToLocation(enemyController->EnemyCharacter->RandomLocationOnNavArea, 10, true, false, false, 0, false);
			}


			if(FVector::Dist(enemyLocation, enemyController->EnemyCharacter->RandomLocationOnNavArea) <= 150)
			{
				hasMovedAwayFromPlayer = true;
				enemyController->EnemyCharacter->GetCapsuleComponent()->SetCollisionProfileName(TEXT("Pawn"));
			}
		}

		return EBTNodeResult::Succeeded;
	}
	else
	{
		return EBTNodeResult::Failed;
	}

	return EBTNodeResult::Failed;
}
