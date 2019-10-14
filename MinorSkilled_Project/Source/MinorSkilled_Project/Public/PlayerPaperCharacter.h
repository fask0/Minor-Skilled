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
	virtual void SetupPlayerInputComponent(class UInputComponent *pInputComponent) override;

protected:
	virtual void BeginPlay() override;
	void UpdatePlayer(float pDeltaTime);
	void UpdateAnimation(FVector pPlayerVelocity);
	void MoveRight(float pValue);
	void Dodge();
	void Attack();
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
		class UPaperFlipbook *DodgeAnimation;

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
	int MeleeAttackFrameToSkip = 0;
	bool CanEnableMeleeHit = true;
	bool IsAttacking = false;
	bool IsAnotherAttackQueued = false;
};
