// Fill out your copyright notice in the Description page of Project Settings.


#include "GameManager.h"
#include "EngineUtils.h"

// Sets default values
AGameManager::AGameManager()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AGameManager::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AGameManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	currentEnemiesInLevel = SpawnedEnemiesInlevel.Num();

	if(roundHasStarted)
	{
		if(currentEnemiesInLevel == 0 && remainingEnemiesInWave == 0)
		{
			//Wave won
			isNextWaveAvailable = true;
		}
	}
}

void AGameManager::BeginNextWave()
{
	isNextWaveAvailable = false;
	currentWave++;
	remainingEnemiesInWave = baseAmountOfEnemiesPerWave * (1 + enemyAmountGrowthPerWave * currentWave);
}

void AGameManager::BeginFirstRound()
{
	EndRound();
	FindEnemySpawnersInLevel();
	currentRound = 1;
	currentWave = 1;
	remainingEnemiesInWave = baseAmountOfEnemiesPerWave * (1 + enemyAmountGrowthPerWave * currentWave);
}

void AGameManager::BeginNextRound()
{
	EndRound();
	FindEnemySpawnersInLevel();
}

void AGameManager::BeginRound(int pRoundNum)
{
	EndRound();
	FindEnemySpawnersInLevel();
}

void AGameManager::EndRound()
{
	for(int i = SpawnedEnemiesInlevel.Num() - 1; i >= 0; i--)
	{
		SpawnedEnemiesInlevel[i]->Destroy();
	}
	roundHasStarted = false;
}

void AGameManager::FindEnemySpawnersInLevel()
{
	enemySpawnersInLevel.Empty();

	UWorld *world = GetWorld();
	for(TActorIterator<AEnemySpawner> It(world, AEnemySpawner::StaticClass()); It; ++It)
	{
		AEnemySpawner *actor = *It;
		if(actor)
		{
			enemySpawnersInLevel.Add(actor);
			actor->GameManager = this;
		}
	}

	roundHasStarted = true;
}

bool AGameManager::SpawnEnemyAtRandomSpawner()
{
	int32 spawnerCount = enemySpawnersInLevel.Num();
	if(spawnerCount == 0) return false;
	if(currentEnemiesInLevel >= maxSpawnedEnemies) return false;
	if(remainingEnemiesInWave == 0) return false;

	int32 randomNumber = FMath::RandRange(0, spawnerCount - 1);
	AEnemySpawner *spawner = enemySpawnersInLevel[randomNumber];
	spawner->SpawnRandomEnemy();
	remainingEnemiesInWave--;
	return true;
}

float AGameManager::GetEnemyHealthScaling()
{
	return 0;
}

float AGameManager::GetEnemyDamageScaling()
{
	return 0;
}

