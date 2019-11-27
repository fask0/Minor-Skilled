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
	AEnemyPaperCharacter();
	class APlayerPaperCharacter *playerCharacter;

	UPROPERTY(EditAnywhere, Category = Behaviour)
		class UBehaviorTree *BehaviourTree;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Enum)
		EnemyAIBehaviour AIBehaviour;

	virtual void BeginPlay() override;
	virtual void Tick(float pDeltaTime) override;
	void TurnAround();
	void TakeDamage(int pDamage, FVector pPlayerForward, float pKnockbackForce);
	UFUNCTION(BlueprintImplementableEvent, Category = Damage)
		void OnTakeDamageEvent(int pDamage);
	UFUNCTION(BlueprintImplementableEvent)
		void GetRandomLocationInNavArea(float pRadius);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement)
		FVector RandomLocationOnNavArea;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement)
		FVector CurrTargetLocation;
	bool IsInAir;
	bool ShouldWait;
	bool ShouldDropDown;

	UFUNCTION(BlueprintCallable)
		void SetShouldWait(bool pToggle);
	UFUNCTION(BlueprintCallable)
		void SetShouldDropDown(bool pToggle);

	void Attack();

	void RangeAttack();
	UFUNCTION(BlueprintImplementableEvent)
		void OnCraeteProjectileEvent(FVector pPlayerLocation);

protected:
	void UpdateEnemy(float pDeltaTime);
	void UpdateAnimation(FVector pVelocity);
	void Hit();
	void Shoot();

	UFUNCTION()
		void OnMeleeOverlapBegin(UPrimitiveComponent *OverlappedComponent,
								 AActor *OtherActor,
								 UPrimitiveComponent *OtherComp,
								 int32 OtherBodyIndex,
								 bool bFromSweep,
								 const FHitResult &SweepResult);

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
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animations)
		class UPaperFlipbook *AttackAnimation;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animations)
		bool ShouldChangeSpriteLocationWhenAttacking = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animations)
		FVector AttackAnimationLocation;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animations)
		FVector OriginalSpriteLocation;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class UCapsuleComponent *MeleeAttackHitBox;

	//Melee Attack
	UPROPERTY(EditAnywhere, Category = "Melee Attack")
		float MeleeDamage;
	UPROPERTY(EditAnywhere, Category = "Melee Attack")
		float MeleeAttackKnockback;
	UPROPERTY(EditAnywhere, Category = "Melee Attack")
		FVector MeleeAttackFrameRange;

	FVector PlayerForward;
	float KnockbackTime = 0;
	bool IsKnockbacked = false;
	bool IsAttacking = false;
	bool AlreadyDidDamgeToPlayerThisAnimation = false;

	//Range Attack
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Range Attack")
		int RangedDamage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Range Attack")
		float RangedKnockback;
	UPROPERTY(EditAnywhere, Category = "Range Attack")
		int AnimationFrameToCreateProjectile;

	bool isRangeAttacking = false;
	bool hasProjectileBeenCrated = false;
};
