// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerPawn.h"
#include "PaperFlipbookComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "Camera/CameraComponent.h"
#include "Components/InputComponent.h"

// Sets default values
APlayerPawn::APlayerPawn()
{
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = false;

	capsuleComponent = CreateDefaultSubobject<UCapsuleComponent>("CapsuleComponent");
	capsuleComponent->SetCapsuleHalfHeight(16.0f);
	capsuleComponent->SetCapsuleRadius(8.0f);

	cameraBoom = CreateDefaultSubobject<USpringArmComponent>("CameraBoom");
	cameraBoom->SetupAttachment(RootComponent);
	cameraBoom->TargetArmLength = 500.0f;
	cameraBoom->SocketOffset = FVector(0.0f, 0.0f, 75.0f);
	cameraBoom->bAbsoluteRotation = true;
	cameraBoom->bDoCollisionTest = false;
	cameraBoom->RelativeRotation = FRotator(0.0f, -90.0f, 0.0f);

	sideViewCamera = CreateDefaultSubobject<UCameraComponent>("SideViewCamera");
	sideViewCamera->ProjectionMode = ECameraProjectionMode::Orthographic;
	sideViewCamera->OrthoWidth = 512.0f;
	sideViewCamera->SetupAttachment(cameraBoom, USpringArmComponent::SocketName);

	cameraBoom->bAbsoluteRotation = true;
	sideViewCamera->bUsePawnControlRotation = false;
	sideViewCamera->bAutoActivate = true;

	characterMovement = CreateDefaultSubobject<UCharacterMovementComponent>("CharacterMovement");
	characterMovement->bOrientRotationToMovement = false;
	characterMovement->bConstrainToPlane = true;
	characterMovement->SetPlaneConstraintNormal(FVector(0.0f, -1.0f, 0.0f));
	characterMovement->bUseFlatBaseForFloorChecks = true;

	sprite = CreateDefaultSubobject<UPaperFlipbookComponent>("Sprite");
	sprite->SetIsReplicated(true);
	bReplicates = true;

	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void APlayerPawn::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void APlayerPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpdatePlayer();
}

void APlayerPawn::UpdateAnimation()
{
	const FVector playerVelocity = GetVelocity();
	const float speedSqr = playerVelocity.SizeSquared();
	const bool isJumping = (playerVelocity.Z > 0.0f) ? true : false;
	const bool isFalling = (playerVelocity.Z < 0.0f) ? true : false;

	UPaperFlipbook *desiredAnimation;
	if(isJumping) desiredAnimation = jumpingAnimation;
	else if(isFalling) desiredAnimation = fallingAnimation;
	else if(speedSqr > 0.0f) desiredAnimation = runningAnimation;
	else desiredAnimation = idleAnimation;

	if(sprite->GetFlipbook() != desiredAnimation)
		sprite->SetFlipbook(desiredAnimation);
}

void APlayerPawn::MoveRight(float pValue)
{
	APlayerPawn::AddMovementInput(FVector(1.0f, 0.0f, 0.0f), pValue);
}

void APlayerPawn::Jump()
{}

void APlayerPawn::StopJumping()
{}

void APlayerPawn::UpdatePlayer()
{
	UpdateAnimation();

	const FVector playerVelocity = GetVelocity();
	float direction = playerVelocity.X;
	if(Controller != NULL)
		if(direction < 0.0f)
			Controller->SetControlRotation(FRotator(0.0f, 180.0f, 0.0f));
		else if(direction > 0.0f)
			Controller->SetControlRotation(FRotator(0.0f, 0.0f, 0.0f));
}

void APlayerPawn::TouchStarted(const ETouchIndex::Type pFingerIndex, const FVector pLocation)
{
	Jump();
}

void APlayerPawn::TouchStopped(const ETouchIndex::Type pFingerIndex, const FVector pLocation)
{
	StopJumping();
}

// Called to bind functionality to input
void APlayerPawn::SetupPlayerInputComponent(UInputComponent *PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &APlayerPawn::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &APlayerPawn::StopJumping);

	PlayerInputComponent->BindAxis("MoveRight", this, &APlayerPawn::MoveRight);

	PlayerInputComponent->BindTouch(IE_Pressed, this, &APlayerPawn::TouchStarted);
	PlayerInputComponent->BindTouch(IE_Released, this, &APlayerPawn::TouchStopped);
}

