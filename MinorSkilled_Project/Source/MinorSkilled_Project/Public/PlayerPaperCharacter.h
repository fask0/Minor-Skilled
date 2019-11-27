// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PaperCharacter.h"
#include "PlayerPaperCharacter.generated.h"

/**
 *
 */

UCLASS()
class MINORSKILLED_PROJECT_API APlayerPaperCharacter : public APaperCharacter
{
	GENERATED_BODY()

public:
	APlayerPaperCharacter();

	virtual void Tick(float pDeltaTime) override;
	void TakeDamage(int pDamage, FVector pEnemyForward, float pKnockbackForce);
	UFUNCTION(BlueprintImplementableEvent, Category = Damage)
		void OnTakeDamageEvent(int pDamage);
	virtual void SetupPlayerInputComponent(class UInputComponent *pInputComponent) override;

protected:
	virtual void BeginPlay() override;
	void UpdatePlayer(float pDeltaTime);
	void UpdateAnimation(FVector pPlayerVelocity);
	void MoveRight(float pValue);
	virtual void Jump() override;
	void Dodge();
	void Attack();
	void StartAttack();
	void StopAttack();
	void UpdateAttacking(float pDeltaTime);
	void Hit();
	UFUNCTION()
		void OnMeleeOverlapBegin(UPrimitiveComponent *OverlappedComponent,
								 AActor *OtherActor,
								 UPrimitiveComponent *OtherComp,
								 int32 OtherBodyIndex,
								 bool bFromSweep,
								 const FHitResult &SweepResult);

	   //Camera
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class UCameraComponent *SideViewCamera;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class USpringArmComponent *CameraBoom;

	//Flipbooks
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animations)
		class UPaperFlipbook *IdleAnimation;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animations)
		class UPaperFlipbook *IdleAnimationWithSword;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animations)
		class UPaperFlipbook *RunningAnimation;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animations)
		class UPaperFlipbook *RunningAnimationWithSword;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animations)
		class UPaperFlipbook *JumpingAnimation;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animations)
		class UPaperFlipbook *FallingAnimation;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animations)
		class UPaperFlipbook *LandingAnimation;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animations)
		class UPaperFlipbook *AttackAnimation;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animations)
		class UPaperFlipbook *DrawSwordAnimation;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animations)
		class UPaperFlipbook *SheathSwordAnimation;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animations)
		class UPaperFlipbook *SlideAnimation;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animations)
		class UPaperFlipbook *StandUpAnimation;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class UCapsuleComponent *MeleeAttackHitBox;

	UPROPERTY(EditAnywhere, Category = "Character Movement")
		float DodgeForce;
	UPROPERTY(EditAnywhere, Category = "Character Movement")
		float DodgeDuration;

	class UCharacterMovementComponent *MovementComponent;
	float DodgeTime;
	float OriginalMaxWalkSpeed;
	bool IsDodging = false;

	//Melee Attack
	UPROPERTY(EditAnywhere, Category = "Melee Attack")
		TArray<int> ComboEndKeyframes;
	UPROPERTY(EditAnywhere, Category = "Melee Attack")
		float SwingMomentum;
	UPROPERTY(EditAnywhere, Category = "Melee Attack")
		float MeleeBaseDamage;
	UPROPERTY(EditAnywhere, Category = "Melee Attack")
		float MeleeMultiplierPerCombo;
	UPROPERTY(EditAnywhere, Category = "Melee Attack")
		float MeleeAttackKnockback;

	int MeleeAttackFrameToSkip = 0;
	int CurrentComboCount = 0;
	int FramesAfterKnockback = 0;
	FVector EnemyForward;
	float KnockbackTime = 0;
	bool IsKnockbacked = false;
	bool IsInCombat = false;
	bool ShouldAttack = false;
	bool ClearAttackQueue = true;
	bool CanEnableMeleeHit = true;
	bool IsAttacking = false;
	bool IsAnotherAttackQueued = false;
	bool ActiveIFrames = false;
};
