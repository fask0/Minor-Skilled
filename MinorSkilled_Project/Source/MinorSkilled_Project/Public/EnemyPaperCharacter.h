// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PaperCharacter.h"
#include "EnemyPaperCharacter.generated.h"

/**
 *
 */

UENUM(BlueprintType)
enum class EnemyAIBehaviour : uint8
{
	AlwaysFollow,
	DontAlwaysFollow
};

UCLASS()
class MINORSKILLED_PROJECT_API AEnemyPaperCharacter : public APaperCharacter
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category = Behaviour)
		class UBehaviorTree *BehaviourTree;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Enum)
		EnemyAIBehaviour AIBehaviour;

	virtual void Tick(float pDeltaTime) override;
	void TakeDamage(int pDamage, FVector pPlayerForward, float pKnockbackForce);
	UFUNCTION(BlueprintImplementableEvent, Category = Damage)
		void OnTakeDamageEvent(int pDamage);
	bool IsInAir;
	bool ShouldWait;

	UFUNCTION(BlueprintCallable)
		void SetShouldWait(bool pToggle);

protected:
	void UpdateEnemy(float pDeltaTime);
	void UpdateAnimation(FVector pVelocity);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animations)
		class UPaperFlipbook *IdleAnimation;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animations)
		class UPaperFlipbook *RunningAnimation;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animations)
		class UPaperFlipbook *JumpingAnimation;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animations)
		class UPaperFlipbook *FallingAnimation;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animations)
		class UPaperFlipbook *LandingAnimation;


	FVector PlayerForward;
	float KnockbackTime = 0;
	bool IsKnockbacked = false;
};
