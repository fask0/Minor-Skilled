// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PaperCharacter.h"
#include "EnemyPaperCharacter.generated.h"

/**
 *
 */

UENUM(BlueprintType)
enum class EnemyType : uint8
{
	MeleeSkeleton,
	RangedWizard,
	RevivingMeleeSkeleton,
	CircleAttackRangedWizard
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
		EnemyType TypeOfEnemy;

	virtual void BeginPlay() override;
	virtual void Tick(float pDeltaTime) override;
	virtual void Destroyed() override;
	UFUNCTION(BlueprintCallable)
		void Init();
	void SetEnemyType(EnemyType pEnemyType);
	void TurnAround();
	void TakeDamage(int pDamage, FVector pPlayerForward, float pKnockbackForce);
	UFUNCTION(BlueprintImplementableEvent, Category = Damage)
		void OnTakeDamageEvent(int pDamage);
	UFUNCTION(BlueprintImplementableEvent)
		void GetRandomLocationInNavArea(float pRadius);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement)
		FVector RandomLocationOnNavArea;
	UPROPERTY(BlueprintReadWrite)
		FVector CurrTargetLocation;
	void SetCurrentTargetLocation(FVector pNewTargetLocation);
	UFUNCTION(BlueprintCallable)
		FVector GetCurrentTargetLocation() { return CurrTargetLocation; }

	bool IsInAir;
	bool ShouldWait;
	bool ShouldDropDown;
	UPROPERTY(BlueprintReadWrite)
		bool BehaviourIsPaused;

	UFUNCTION(BlueprintCallable)
		void SetShouldWait(bool pToggle);
	UFUNCTION(BlueprintCallable)
		void SetShouldDropDown(bool pToggle);

	void Attack();

	void RangeAttack();
	UFUNCTION(BlueprintImplementableEvent)
		void OnCraeteProjectileEvent(FVector pPlayerLocation);
	UFUNCTION(BlueprintImplementableEvent)
		void OnBeginPlayAfterCPPSetup();

	int GetCurrentHealth() { return currentHealth; }

protected:
	void UpdateEnemy(float pDeltaTime);
	void UpdateAnimation(FVector pVelocity);
	void Hit();
	void Shoot();
	void CueDeath();
	void Die();
	void Revive();

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
		class UPaperFlipbook *RangedCircleAttackAnimation;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animations)
		class UPaperFlipbook *DeathAnimation;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animations)
		class UPaperFlipbook *ReviveAnimation;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animations)
		bool ShouldChangeSpriteLocationWhenAttacking = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animations)
		FVector AttackAnimationLocation;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animations)
		FVector OriginalSpriteLocation;

	ECollisionEnabled::Type originalCollisionEnabled;

	//Melee Attack
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Melee Attack", meta = (AllowPrivateAccess = "true"))
		class UCapsuleComponent *MeleeAttackHitBox;
	UPROPERTY(EditAnywhere, Category = "Melee Attack")
		int baseMeleeDamage;
	UPROPERTY(EditAnywhere, Category = "Melee Attack")
		float meleeAttackKnockback;
	UPROPERTY(EditAnywhere, Category = "Melee Attack")
		FVector meleeAttackFrameRange;

	FVector playerForward;
	float knockbackTime = 0;
	bool isKnockbacked = false;
	bool isAttacking = false;
	bool alreadyDidDamgeToPlayerThisAnimation = false;

	//Range Attack
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Range Attack")
		int baseRangedDamage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Range Attack")
		float rangedKnockback;
	UPROPERTY(EditAnywhere, Category = "Range Attack")
		int animationFrameToCreateProjectile;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Range Attack - Circle Attack")
		bool canUseCircleAttack;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Range Attack - Circle Attack")
		float circleAttackDistFromCentre;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Range Attack - Circle Attack")
		int circleAttackProjectileAmount;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Range Attack - Circle Attack")
		float delayBeforeShooting;

	UFUNCTION(BlueprintImplementableEvent)
		void OnPrepareCircleAttack();
	UPROPERTY(BlueprintReadWrite)
		TArray<FVector> projectileLocationsInCircle;
	bool isRangeAttacking = false;
	bool hasProjectileBeenCrated = false;

	UFUNCTION(BlueprintCallable)
		void SetGameManager(AGameManager *pGameManager);
	class AGameManager *gameManager;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "General Stats")
		int baseHealth;
	UPROPERTY(BlueprintReadWrite)
		int currentHealth;
	UPROPERTY(BlueprintReadWrite)
		int maxHealth;
	bool shouldDie;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "General Stats")
		bool canRevive;
	bool isReviving;

	UPROPERTY(BlueprintReadWrite)
		float behaviourActivationDelay;
};
