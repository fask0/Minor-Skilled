// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "PlayerPawn.generated.h"

UCLASS()
class MINORSKILLED_PROJECT_API APlayerPawn : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	APlayerPawn();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(Category = Character, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		class UCapsuleComponent *capsuleComponent;
	UPROPERTY(Category = Character, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		class UCharacterMovementComponent *characterMovement;
	UPROPERTY(Category = Character, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		class UPaperFlipbookComponent *sprite;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class UCameraComponent *sideViewCamera;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class USpringArmComponent *cameraBoom;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animations)
		class UPaperFlipbook *idleAnimation;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animations)
		class UPaperFlipbook *runningAnimation;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animations)
		class UPaperFlipbook *jumpingAnimation;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animations)
		class UPaperFlipbook *fallingAnimation;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animations)
		class UPaperFlipbook *attackAnimation;

	void UpdateAnimation();
	void MoveRight(float pValue);
	void Jump();
	void StopJumping();
	void UpdatePlayer();

	void TouchStarted(const ETouchIndex::Type pFingerIndex, const FVector pLocation);
	void TouchStopped(const ETouchIndex::Type pFingerIndex, const FVector pLocation);

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent *pInputComponent) override;

	FORCEINLINE class UCameraComponent *GetSideViewCameraComponent() const { return sideViewCamera; }
	FORCEINLINE class USpringArmComponent *GetCameraBoom() const { return cameraBoom; }
};
