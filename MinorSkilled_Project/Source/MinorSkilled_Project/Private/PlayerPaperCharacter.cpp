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
#include "GameFramework/Pawn.h"
#include "GameFramework/Controller.h"
#include "Components/CapsuleComponent.h"
#include "Engine/Engine.h"
#include "EnemyPaperCharacter.h"

APlayerPaperCharacter::APlayerPaperCharacter()
{
	MeleeAttackHitBox = CreateDefaultSubobject<UCapsuleComponent>("MeleeAttackHitBox");
	MeleeAttackHitBox->SetupAttachment(RootComponent);

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
	pInputComponent->BindAction("Attack", IE_Pressed, this, &APlayerPaperCharacter::Attack);
	pInputComponent->BindAxis("MoveRight", this, &APlayerPaperCharacter::MoveRight);
}

void APlayerPaperCharacter::BeginPlay()
{
	APaperCharacter::BeginPlay();

	MovementComponent = Cast<UCharacterMovementComponent>(GetCharacterMovement());
	OriginalMaxWalkSpeed = MovementComponent->MaxWalkSpeed;
	MeleeAttackHitBox->OnComponentBeginOverlap.AddDynamic(this, &APlayerPaperCharacter::OnMeleeOverlapBegin);
	MeleeAttackHitBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
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
	UpdateAttacking(pDeltaTime);

	float direction = playerVelocity.X;
	if(Controller != nullptr)
		if(direction < 0)
			Controller->SetControlRotation(FRotator(0, 180, 0));
		else if(direction > 0)
			Controller->SetControlRotation(FRotator(0, 0, 0));

	SetActorLocation(FVector(GetActorLocation().X, 0, GetActorLocation().Z));
}

void APlayerPaperCharacter::UpdateAnimation(FVector pPlayerVelocity)
{
	const float speedSqr = pPlayerVelocity.SizeSquared();
	const bool isJumping = (pPlayerVelocity.Z > 0) ? true : false;
	const bool isFalling = (pPlayerVelocity.Z < 0) ? true : false;
	const float animationPositionInFrames = GetSprite()->GetPlaybackPositionInFrames();

	UPaperFlipbook *desiredAnimation;
	if(IsAttacking)
	{
		desiredAnimation = AttackAnimation;
		MovementComponent->MaxWalkSpeed = OriginalMaxWalkSpeed * 0.05f;
		MeleeAttackHitBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		Hit();
	}
	else if(isJumping)
	{
		desiredAnimation = JumpingAnimation;
	}
	else if(isFalling)
	{
		desiredAnimation = FallingAnimation;
	}
	else if(GetSprite()->GetFlipbook() == FallingAnimation || (GetSprite()->GetFlipbook() == LandingAnimation && animationPositionInFrames < GetSprite()->GetFlipbookLengthInFrames() - 1))
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
	if(IsDodging || IsAttacking) return;

	DodgeTime = DodgeDuration;

	GetSprite()->SetFlipbook(DodgeAnimation);
	GetSprite()->SetLooping(false);
	GetSprite()->Play();

	MovementComponent->MaxWalkSpeed = (GetVelocity().X == 0) ? DodgeForce : OriginalMaxWalkSpeed + DodgeForce;

	IsDodging = true;
}

void APlayerPaperCharacter::Attack()
{
	if(IsDodging) return;
	if(IsAttacking)
		IsAnotherAttackQueued = true;

	IsAttacking = true;
}

void APlayerPaperCharacter::UpdateAttacking(float pDeltaTime)
{
	if(!IsAttacking && GetSprite()->GetFlipbook() != AttackAnimation) return;

	const float animationPositionInFrames = GetSprite()->GetPlaybackPositionInFrames();
	if(!IsAnotherAttackQueued)
	{
		for(int i = 0; i < ComboEndKeyframes.Num(); ++i)
		{
			if(animationPositionInFrames == ComboEndKeyframes[i])
			{
				if(animationPositionInFrames == MeleeAttackFrameToSkip) break;
				IsAttacking = false;
				CanEnableMeleeHit = true;
				MeleeAttackFrameToSkip = 0;
				MovementComponent->MaxWalkSpeed = OriginalMaxWalkSpeed;
				return;
			}
		}
	}
	else
	{
		for(int i = 0; i < ComboEndKeyframes.Num(); ++i)
		{
			if(animationPositionInFrames == ComboEndKeyframes[i])
			{
				IsAnotherAttackQueued = false;
				CanEnableMeleeHit = true;
				MeleeAttackFrameToSkip = animationPositionInFrames;
				return;
			}
		}
	}
}

void APlayerPaperCharacter::Hit()
{
	const float animationPositionInFrames = GetSprite()->GetPlaybackPositionInFrames();
	for(int i = 0; i < ComboEndKeyframes.Num(); ++i)
	{
		if(animationPositionInFrames == ComboEndKeyframes[i] - 2)
		{
			MovementComponent->MaxWalkSpeed = OriginalMaxWalkSpeed;
			AddMovementInput(this->GetActorForwardVector(), SwingMomentum);
			if(CanEnableMeleeHit)
			{
				MeleeAttackHitBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
				CanEnableMeleeHit = false;
			}
			break;
		}
	}
}

void APlayerPaperCharacter::OnMeleeOverlapBegin(UPrimitiveComponent *OverlappedComponent, AActor *OtherActor, UPrimitiveComponent *OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult)
{
	AEnemyPaperCharacter *enemy = Cast<AEnemyPaperCharacter>(OtherActor);
	if(!enemy) return;
	enemy->TakeDamage(10, GetActorForwardVector(), 0.2f);
	GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Cyan, "Hitting enemy!");
}
