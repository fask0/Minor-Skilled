// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EnemySpawner.generated.h"

UCLASS()
class MINORSKILLED_PROJECT_API AEnemySpawner : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AEnemySpawner();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintImplementableEvent)
		void OnSpawnMeleeEnemy(float pBehaviourDelay, int pEnemyType);
	UFUNCTION(BlueprintImplementableEvent)
		void OnSpawnRangedEnemy(float pBehaviourDelay, int pEnemyType);
	UFUNCTION(BlueprintImplementableEvent)
		void OnSpawnRandomEnemy(float pBehaviourDelay, int pEnemyType);

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void SpawnMeleeEnemy();
	void SpawnRangedEnemy();
	void SpawnRandomEnemy();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn")
		float BehaviourActivationDelay;

	UPROPERTY(BlueprintReadWrite)
		class AGameManager *GameManager;
};
