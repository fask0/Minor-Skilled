// Fill out your copyright notice in the Description page of Project Settings.


#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyAllTypes.h"
#include "EnemyAIController.h"
#include "EnemyPaperCharacter.h"
#include "BTService_CheckForPlayer.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Pawn.h"
#include "PlayerPaperCharacter.h"

UBTService_CheckForPlayer::UBTService_CheckForPlayer()
{
	bCreateNodeInstance = true;
}

void UBTService_CheckForPlayer::TickNode(UBehaviorTreeComponent &pOwnerComp, uint8 *pNodeMemory, float pDeltaSeconds)
{
	AEnemyAIController *enemyController = Cast<AEnemyAIController>(pOwnerComp.GetAIOwner());
	if(enemyController)
	{
		APlayerPaperCharacter *player = Cast<APlayerPaperCharacter>(GetWorld()->GetFirstPlayerController()->GetPawn());

		if(player)
		{
			pOwnerComp.GetBlackboardComponent()->SetValue<UBlackboardKeyType_Object>(enemyController->TargetKeyID, player);
			pOwnerComp.GetBlackboardComponent()->SetValue<UBlackboardKeyType_Vector>(enemyController->TargetLocationKeyID, player->GetTargetLocation());
		}
	}
}
