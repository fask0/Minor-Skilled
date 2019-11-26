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

void APlayerPaperCharacter::TakeDamage(int pDamage, FVector pEnemyForward, float pKnockbackForce)
{
	if(ActiveIFrames) return;

	OnTakeDamageEvent(pDamage);
	KnockbackTime = pKnockbackForce;
	EnemyForward = pEnemyForward;
	if(KnockbackTime > 0)
		IsKnockbacked = true;
}

void APlayerPaperCharacter::SetupPlayerInputComponent(class UInputComponent *pInputComponent)
{
	Super::SetupPlayerInputComponent(pInputComponent);

	pInputComponent->BindAction("Jump", IE_Pressed, this, &APlayerPaperCharacter::Jump);
	pInputComponent->BindAction("Dodge", IE_Pressed, this, &APlayerPaperCharacter::Dodge);
	pInputComponent->BindAction("Attack", IE_Pressed, this, &APlayerPaperCharacter::StartAttack);
	pInputComponent->BindAction("Attack", IE_Released, this, &APlayerPaperCharacter::StopAttack);
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

	if(IsKnockbacked)
	{
		if(KnockbackTime <= 0)
		{
			IsKnockbacked = false;
		}
		else
		{
			KnockbackTime -= pDeltaTime;
			AddMovementInput(EnemyForward, 100);
			FramesAfterKnockback = 0;
		}
	}
	else
	{
		FramesAfterKnockback++;
	}

	if(IsDodging)
	{
		AddMovementInput(this->GetActorForwardVector(), DodgeForce);

		DodgeTime -= pDeltaTime;
		if(DodgeTime <= 0)
		{
			MovementComponent->MaxWalkSpeed = OriginalMaxWalkSpeed;
			ActiveIFrames = false;
			IsDodging = false;
		}
		else
		{
			return;
		}
	}

	const FVector playerVelocity = GetVelocity();
	Attack();
	UpdateAnimation(playerVelocity);
	UpdateAttacking(pDeltaTime);

	float direction = playerVelocity.X;
	if(Controller != nullptr && FramesAfterKnockback > 20)
	{
		if(direction < 0)
			Controller->SetControlRotation(FRotator(0, 180, 0));
		else if(direction > 0)
			Controller->SetControlRotation(FRotator(0, 0, 0));
	}

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
		MovementComponent->MaxWalkSpeed = OriginalMaxWalkSpeed * 0.01f;
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

void APlayerPaperCharacter::Jump()
{
	if(IsDodging || IsAttacking) return;
	APaperCharacter::Jump();
}

void APlayerPaperCharacter::Dodge()
{
	if(IsDodging || IsAttacking) return;

	DodgeTime = DodgeDuration;

	GetSprite()->SetFlipbook(SlideAnimation);
	GetSprite()->SetLooping(false);
	GetSprite()->Play();

	MovementComponent->MaxWalkSpeed = (GetVelocity().X == 0) ? DodgeForce : OriginalMaxWalkSpeed + DodgeForce;

	ActiveIFrames = true;
	IsDodging = true;
}

void APlayerPaperCharacter::Attack()
{
	if(IsDodging || !ShouldAttack) return;
	const float animationPositionInFrames = GetSprite()->GetPlaybackPositionInFrames();
	if(IsAttacking)
		IsAnotherAttackQueued = true;

	IsAttacking = true;
}

void APlayerPaperCharacter::StartAttack()
{
	ShouldAttack = true;
}

void APlayerPaperCharacter::StopAttack()
{
	ShouldAttack = false;
	IsAnotherAttackQueued = false;
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

	//Check combo
	const float animationPositionInFrames = GetSprite()->GetPlaybackPositionInFrames();
	CurrentComboCount = 0;
	for(int i = 0; i < ComboEndKeyframes.Num(); ++i)
		if(animationPositionInFrames > ComboEndKeyframes[i])
			CurrentComboCount++;

	enemy->TakeDamage(FMath::RoundFromZero(MeleeBaseDamage + MeleeBaseDamage * MeleeMultiplierPerCombo * CurrentComboCount), GetActorForwardVector(), MeleeAttackKnockback);
}
