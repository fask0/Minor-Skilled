// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EnemySpawner.h"
#include "EnemyPaperCharacter.h"
#include "GameManager.generated.h"

UCLASS()
class MINORSKILLED_PROJECT_API AGameManager : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AGameManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	TArray<AEnemySpawner *> enemySpawnersInLevel;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemies")
		int maxSpawnedEnemies;
	UPROPERTY(BlueprintReadWrite)
		int currentEnemiesInLevel;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemies")
		float enemyHealthGrowthPerWave;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemies")
		float enemyHealthGrowthPerRound;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemies")
		float enemyDamageGrowthPerWave;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemies")
		float enemyDamageGrowthPerRound;

	UPROPERTY(BlueprintReadWrite)
		bool roundHasStarted;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Waves and Rounds")
		int baseWavesPerRound;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Waves and Rounds")
		int baseAmountOfEnemiesPerWave;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Waves and Rounds")
		float enemyAmountGrowthPerWave;
	int currentWave = 0;
	int currentRound = 0;
	int remainingEnemiesInWave = 0;

	UPROPERTY(BlueprintReadWrite)
		bool isNextWaveAvailable;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
		void BeginNextWave();
	UFUNCTION(BlueprintCallable)
		void BeginFirstRound();
	UFUNCTION(BlueprintCallable)
		void BeginNextRound();
	UFUNCTION(BlueprintCallable)
		void BeginRound(int pRoundNum);
	UFUNCTION(BlueprintCallable)
		void EndRound();

	UFUNCTION(BlueprintCallable)
		void FindEnemySpawnersInLevel();
	UFUNCTION(BlueprintCallable)
		bool SpawnEnemyAtRandomSpawner();

	TArray<AEnemyPaperCharacter *> SpawnedEnemiesInlevel;

	UFUNCTION(BlueprintCallable)
		int GetCurrentWave() { return currentWave; }
	UFUNCTION(BlueprintCallable)
		int GetCurrentRound() { return currentRound; }

	UFUNCTION(BlueprintCallable)
		float GetEnemyHealthScaling();
	UFUNCTION(BlueprintCallable)
		float GetEnemyDamageScaling();
};
