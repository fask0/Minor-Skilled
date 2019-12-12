// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyPaperCharacter.h"
#include "PaperCharacter.h"
#include "PaperFlipbookComponent.h"
#include "UserWidget.h"
#include "Components/CapsuleComponent.h"
#include "Components/PrimitiveComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "PlayerPaperCharacter.h"
#include "GameFramework/Actor.h"
#include "Engine/Engine.h"
#include "GameFramework/MovementComponent.h"
#include "GameManager.h"

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
	ShouldDropDown = false;
	originalCollisionEnabled = GetCapsuleComponent()->GetCollisionEnabled();
}

void AEnemyPaperCharacter::Tick(float pDeltaTime)
{
	Super::Tick(pDeltaTime);

	if(behaviourActivationDelay > 0)
	{
		BehaviourIsPaused = true;
		behaviourActivationDelay -= pDeltaTime;
		if(behaviourActivationDelay <= 0)
			BehaviourIsPaused = false;
	}

	UpdateEnemy(pDeltaTime);
}

void AEnemyPaperCharacter::Destroyed()
{
	APaperCharacter::Destroyed();

	if(gameManager)
	{
		gameManager->SpawnedEnemiesInlevel.Remove(this);
	}
}

void AEnemyPaperCharacter::Init()
{
	switch(TypeOfEnemy)
	{
		case EnemyType::MeleeSkeleton:
			canRevive = false;
			break;
		case EnemyType::RangedWizard:
			canUseCircleAttack = false;
			break;
		case EnemyType::RevivingMeleeSkeleton:
			canRevive = true;
			break;
		case EnemyType::CircleAttackRangedWizard:
			canUseCircleAttack = true;
			break;
		default:
			break;
	}

	if(gameManager)
	{
		gameManager->SpawnedEnemiesInlevel.Add(this);
		currentHealth = baseHealth * (1 + gameManager->GetEnemyHealthScaling());
	}
	else
		currentHealth = baseHealth;
	maxHealth = currentHealth;

	OnBeginPlayAfterCPPSetup();
}

void AEnemyPaperCharacter::SetEnemyType(EnemyType pEnemyType)
{
	TypeOfEnemy = pEnemyType;
}

void AEnemyPaperCharacter::TurnAround()
{
	GetCharacterMovement()->MoveUpdatedComponent(FVector(0, 0, 0), FRotator(0, GetActorRotation().Yaw + 180, 0), false);
}

void AEnemyPaperCharacter::TakeDamage(int pDamage, FVector pPlayerForward, float pKnockbackForce)
{
	OnTakeDamageEvent(pDamage);

	if(currentHealth - pDamage <= 0)
	{
		CueDeath();
		currentHealth = 0;
		knockbackTime = 0;
		isKnockbacked = false;
		return;
	}

	currentHealth -= pDamage;
	knockbackTime = pKnockbackForce;
	playerForward = pPlayerForward;
	if(knockbackTime > 0)
		isKnockbacked = true;
}

void AEnemyPaperCharacter::SetCurrentTargetLocation(FVector pNewTargetLocation)
{
	CurrTargetLocation = pNewTargetLocation;
}

void AEnemyPaperCharacter::SetShouldWait(bool pToggle)
{
	ShouldWait = pToggle;
}

void AEnemyPaperCharacter::SetShouldDropDown(bool pToggle)
{
	ShouldDropDown = pToggle;
}

void AEnemyPaperCharacter::Attack()
{
	isAttacking = true;
}

void AEnemyPaperCharacter::RangeAttack()
{
	isRangeAttacking = true;
}

void AEnemyPaperCharacter::UpdateEnemy(float pDeltaTime)
{
	const FVector velocity = GetVelocity();

	if(isKnockbacked)
	{
		knockbackTime -= pDeltaTime;
		AddMovementInput(playerForward, 100);

		if(knockbackTime <= 0)
			isKnockbacked = false;
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

	if(isReviving)
	{
		desiredAnimation = ReviveAnimation;
		Revive();
	}
	else if(shouldDie)
	{
		desiredAnimation = DeathAnimation;
		Die();
	}
	else if(isAttacking)
	{
		desiredAnimation = AttackAnimation;
		Hit();
	}
	else if(isRangeAttacking)
	{
		if(canUseCircleAttack)
			desiredAnimation = RangedCircleAttackAnimation;
		else
			desiredAnimation = AttackAnimation;
		Shoot();
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

		if(desiredAnimation != IdleAnimation || desiredAnimation != RunningAnimation)
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
	if(GetSprite()->GetFlipbook() != AttackAnimation) return;
	const float animationPositionInFrames = GetSprite()->GetPlaybackPositionInFrames();

	MeleeAttackHitBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	if(ShouldChangeSpriteLocationWhenAttacking)
		GetSprite()->SetRelativeLocation(AttackAnimationLocation);

	if(animationPositionInFrames >= meleeAttackFrameRange.X && animationPositionInFrames <= meleeAttackFrameRange.Y)
	{
		MeleeAttackHitBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	}

	if(animationPositionInFrames >= GetSprite()->GetFlipbookLengthInFrames() - 1)
	{
		alreadyDidDamgeToPlayerThisAnimation = false;
		isAttacking = false;
		MeleeAttackHitBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}

void AEnemyPaperCharacter::Shoot()
{
	if(GetSprite()->GetFlipbook() != AttackAnimation && GetSprite()->GetFlipbook() != RangedCircleAttackAnimation) return;
	const float animationPositionInFrames = GetSprite()->GetPlaybackPositionInFrames();

	if(!hasProjectileBeenCrated && animationPositionInFrames == animationFrameToCreateProjectile)
	{
		if(canUseCircleAttack)
		{
			projectileLocationsInCircle.Empty();
			for(int i = 0; i < circleAttackProjectileAmount; i++)
			{
				float angle = i * 3.141592f * 2 / circleAttackProjectileAmount;
				FVector newPos = FVector(FMath::Cos(angle) * circleAttackDistFromCentre, 0, FMath::Sin(angle) * circleAttackDistFromCentre);
				projectileLocationsInCircle.Add(newPos);
			}
			OnPrepareCircleAttack();
		}
		else
		{
			OnCraeteProjectileEvent(playerCharacter->GetActorLocation());
		}
		hasProjectileBeenCrated = true;
	}

	if(animationPositionInFrames >= GetSprite()->GetFlipbookLengthInFrames() - 1)
	{
		isRangeAttacking = false;
		hasProjectileBeenCrated = false;
	}
}

void AEnemyPaperCharacter::CueDeath()
{
	shouldDie = true;
	currentHealth = 0;
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AEnemyPaperCharacter::Die()
{
	if(GetSprite()->GetFlipbook() != DeathAnimation) return;
	const float animationPositionInFrames = GetSprite()->GetPlaybackPositionInFrames();

	if(animationPositionInFrames == GetSprite()->GetFlipbookLengthInFrames() - 1)
	{
		if(canRevive)
		{
			isReviving = true;
			shouldDie = false;
		}
		else
		{
			Destroy();
		}
	}
}

void AEnemyPaperCharacter::Revive()
{
	if(GetSprite()->GetFlipbook() != ReviveAnimation) return;
	const float animationPositionInFrames = GetSprite()->GetPlaybackPositionInFrames();

	if(animationPositionInFrames == GetSprite()->GetFlipbookLengthInFrames() - 1)
	{
		canRevive = false;
		isReviving = false;
		currentHealth = maxHealth * 0.5f;
		GetCapsuleComponent()->SetCollisionEnabled(originalCollisionEnabled);
	}
}

void AEnemyPaperCharacter::OnMeleeOverlapBegin(UPrimitiveComponent *OverlappedComponent, AActor *OtherActor, UPrimitiveComponent *OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult)
{
	APlayerPaperCharacter *player = Cast<APlayerPaperCharacter>(OtherActor);
	if(!player || alreadyDidDamgeToPlayerThisAnimation) return;

	player->TakeDamage(baseMeleeDamage, GetActorForwardVector(), meleeAttackKnockback);
	alreadyDidDamgeToPlayerThisAnimation = true;
}

void AEnemyPaperCharacter::SetGameManager(AGameManager *pGameManager)
{
	gameManager = pGameManager;
}
