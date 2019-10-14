// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyPaperCharacter.h"
#include "PaperCharacter.h"
#include "PaperFlipbookComponent.h"
#include "UserWidget.h"

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
	if(isJumping)
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
