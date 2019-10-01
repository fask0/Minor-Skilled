// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerPaperCharacter.h"
#include "PaperCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/InputComponent.h"
#include "PaperFlipbookComponent.h"
#include "Components/PrimitiveComponent.h"
#include "GameFramework/Actor.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

APlayerPaperCharacter::APlayerPaperCharacter()
{
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>("CameraBoom");
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 50;
	CameraBoom->SocketOffset = FVector(0, 0, 75);
	CameraBoom->bAbsoluteRotation = true;
	CameraBoom->bDoCollisionTest = false;
	CameraBoom->RelativeRotation = FRotator(0, -90, 0);

	SideViewCamera = CreateDefaultSubobject<UCameraComponent>("SideViewCamera");
	SideViewCamera->ProjectionMode = ECameraProjectionMode::Orthographic;
	SideViewCamera->OrthoWidth = 512;
	SideViewCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	SideViewCamera->bUsePawnControlRotation = false;
	SideViewCamera->bAutoActivate = true;
}

void APlayerPaperCharacter::Tick(float pDeltaTime)
{
	Super::Tick(pDeltaTime);

	UpdatePlayer(pDeltaTime);
}

void APlayerPaperCharacter::SetupPlayerInputComponent(class UInputComponent *pInputComponent)
{
	Super::SetupPlayerInputComponent(pInputComponent);

	pInputComponent->BindAction("Jump", IE_Pressed, this, &APaperCharacter::Jump);
	pInputComponent->BindAction("Dodge", IE_Pressed, this, &APlayerPaperCharacter::Dodge);
	pInputComponent->BindAxis("MoveRight", this, &APlayerPaperCharacter::MoveRight);
}

void APlayerPaperCharacter::BeginPlay()
{
	APaperCharacter::BeginPlay();

	MovementComponent = Cast<UCharacterMovementComponent>(GetCharacterMovement());
	OriginalMaxWalkSpeed = MovementComponent->MaxWalkSpeed;
}

void APlayerPaperCharacter::UpdatePlayer(float pDeltaTime)
{
	const FVector playerVelocity = GetVelocity();

	if(IsDodging)
	{
		AddMovementInput(this->GetActorForwardVector(), DodgeForce);

		DodgeTime -= pDeltaTime;
		if(DodgeTime <= 0)
		{
			MovementComponent->MaxWalkSpeed = OriginalMaxWalkSpeed;
			IsDodging = false;
		}
		else
		{
			return;
		}
	}

	UpdateAnimation(playerVelocity);

	float direction = playerVelocity.X;
	if(Controller != nullptr)
		if(direction < 0)
			Controller->SetControlRotation(FRotator(0, 180, 0));
		else if(direction > 0)
			Controller->SetControlRotation(FRotator(0, 0, 0));
}

void APlayerPaperCharacter::UpdateAnimation(FVector pPlayerVelocity)
{
	const float speedSqr = pPlayerVelocity.SizeSquared();
	const bool isJumping = (pPlayerVelocity.Z > 0) ? true : false;
	const bool isFalling = (pPlayerVelocity.Z < 0) ? true : false;

	UPaperFlipbook *desiredAnimation;
	if(isJumping)
	{
		desiredAnimation = JumpingAnimation;
	}
	else if(isFalling)
	{
		desiredAnimation = FallingAnimation;
	}
	else if(GetSprite()->GetFlipbook() == FallingAnimation ||
		(GetSprite()->GetFlipbook() == LandingAnimation && GetSprite()->GetPlaybackPositionInFrames() < GetSprite()->GetFlipbookLengthInFrames() - 1))
	{
		desiredAnimation = LandingAnimation;
	}
	else if(speedSqr > 0)
	{
		desiredAnimation = RunningAnimation;
	}
	else
	{
		desiredAnimation = IdleAnimation;
	}

	if(GetSprite()->GetFlipbook() != desiredAnimation)
	{
		GetSprite()->SetFlipbook(desiredAnimation);
		GetSprite()->Play();

		if(desiredAnimation == JumpingAnimation ||
		   desiredAnimation == FallingAnimation ||
		   desiredAnimation == LandingAnimation)
		{
			GetSprite()->SetLooping(false);
		}
		else if(!GetSprite()->IsLooping())
		{
			GetSprite()->SetLooping(true);
		}
	}
}

void APlayerPaperCharacter::MoveRight(float pValue)
{
	if(IsDodging) return;
	AddMovementInput(FVector(1, 0, 0), pValue);
}

void APlayerPaperCharacter::Dodge()
{
	if(IsDodging) return;

	DodgeTime = DodgeDuration;

	GetSprite()->SetFlipbook(DodgeAnimation);
	GetSprite()->SetLooping(false);
	GetSprite()->Play();

	MovementComponent->MaxWalkSpeed = (GetVelocity().X == 0) ? DodgeForce : OriginalMaxWalkSpeed + DodgeForce;

	IsDodging = true;
}
