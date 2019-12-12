// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemySpawner.h"

// Sets default values
AEnemySpawner::AEnemySpawner()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AEnemySpawner::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AEnemySpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AEnemySpawner::SpawnMeleeEnemy()
{
	int randNum = FMath::RandRange(0, 2);
	OnSpawnMeleeEnemy(BehaviourActivationDelay, randNum);
}

void AEnemySpawner::SpawnRangedEnemy()
{
	int randNum = FMath::RandRange(0, 2);
	OnSpawnRangedEnemy(BehaviourActivationDelay, randNum);
}

void AEnemySpawner::SpawnRandomEnemy()
{
	int randNum = FMath::RandRange(0, 4);
	OnSpawnRandomEnemy(BehaviourActivationDelay, randNum);
}

