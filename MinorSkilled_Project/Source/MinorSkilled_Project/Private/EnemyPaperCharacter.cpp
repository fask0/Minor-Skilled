// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyPaperCharacter.h"
#include "PaperCharacter.h"
#include "PaperFlipbookComponent.h"
#include "UserWidget.h"
#include "Components/CapsuleComponent.h"
#include "Components/PrimitiveComponent.h"
#include "PlayerPaperCharacter.h"
#include "GameFramework/Actor.h"
#include "Engine/Engine.h"

AEnemyPaperCharacter::AEnemyPaperCharacter()
{
	MeleeAttackHitBox = CreateDefaultSubobject<UCapsuleComponent>("MeleeAttackHitBox");
	MeleeAttackHitBox->SetupAttachment(RootComponent);
}

void AEnemyPaperCharacter::BeginPlay()
{
	APaperCharacter::BeginPlay();

	MeleeAttackHitBox->OnComponentBeginOverlap.AddDynamic(this, &AEnemyPaperCharacter::OnMeleeOverlapBegin);
	MeleeAttackHitBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AEnemyPaperCharacter::Tick(float pDeltaTime)
{
	Super::Tick(pDeltaTime);

	UpdateEnemy(pDeltaTime);
}

void AEnemyPaperCharacter::TakeDamage(int pDamage, FVector pPlayerForward, float pKnockbackForce)
{
	OnTakeDamageEvent(pDamage);
	KnockbackTime = pKnockbackForce;
	PlayerForward = pPlayerForward;
	if(KnockbackTime > 0)
		IsKnockbacked = true;
}

void AEnemyPaperCharacter::SetShouldWait(bool pToggle)
{
	ShouldWait = pToggle;
}

void AEnemyPaperCharacter::Attack()
{
	IsAttacking = true;
}

void AEnemyPaperCharacter::UpdateEnemy(float pDeltaTime)
{
	const FVector velocity = GetVelocity();

	if(IsKnockbacked)
	{
		KnockbackTime -= pDeltaTime;
		AddMovementInput(PlayerForward, 100);

		if(KnockbackTime <= 0)
			IsKnockbacked = false;
	}

	UpdateAnimation(velocity);

	SetActorLocation(FVector(GetActorLocation().X, 0, GetActorLocation().Z));
}

void AEnemyPaperCharacter::UpdateAnimation(FVector pVelocity)
{
	const float speedSqr = pVelocity.SizeSquared();
	const bool isJumping = (pVelocity.Z > 0) ? true : false;
	const bool isFalling = (pVelocity.Z < 0) ? true : false;
	IsInAir = false;

	UPaperFlipbook *desiredAnimation;
	if(ShouldChangeSpriteLocationWhenAttacking)
		GetSprite()->SetRelativeLocation(OriginalSpriteLocation);

	if(IsAttacking)
	{
		desiredAnimation = AttackAnimation;

		MeleeAttackHitBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		Hit();

		if(ShouldChangeSpriteLocationWhenAttacking)
			GetSprite()->SetRelativeLocation(AttackAnimationLocation);
	}
	else if(isJumping)
	{
		IsInAir = true;
		desiredAnimation = JumpingAnimation;
	}
	else if(isFalling)
	{
		IsInAir = true;
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
		   desiredAnimation == LandingAnimation ||
		   desiredAnimation == AttackAnimation)
		{
			GetSprite()->SetLooping(false);
		}
		else if(!GetSprite()->IsLooping())
		{
			GetSprite()->SetLooping(true);
		}
	}
}

void AEnemyPaperCharacter::Hit()
{
	const float animationPositionInFrames = GetSprite()->GetPlaybackPositionInFrames();
	if(animationPositionInFrames >= MeleeAttackFrameRange.X && animationPositionInFrames <= MeleeAttackFrameRange.Y)
	{
		MeleeAttackHitBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	}

	if(animationPositionInFrames >= GetSprite()->GetFlipbookLengthInFrames() - 1)
	{
		AlreadyDidDamgeToPlayerThisAnimation = false;
		IsAttacking = false;
		MeleeAttackHitBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}

void AEnemyPaperCharacter::OnMeleeOverlapBegin(UPrimitiveComponent *OverlappedComponent, AActor *OtherActor, UPrimitiveComponent *OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult)
{
	APlayerPaperCharacter *player = Cast<APlayerPaperCharacter>(OtherActor);
	if(!player || AlreadyDidDamgeToPlayerThisAnimation) return;

	player->TakeDamage(MeleeDamage, GetActorForwardVector(), MeleeAttackKnockback);
	AlreadyDidDamgeToPlayerThisAnimation = true;
}
