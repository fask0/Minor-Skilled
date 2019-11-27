// Fill out your copyright notice in the Description page of Project Settings.


#include "RangedEnemyProjectile.h"
#include "Engine/Engine.h"
#include "PlayerPaperCharacter.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/Character.h"
#include "Components/PrimitiveComponent.h"

void ARangedEnemyProjectile::BeginPlay()
{
	Super::BeginPlay();

	GetCapsuleComponent()->OnComponentBeginOverlap.AddDynamic(this, &ARangedEnemyProjectile::OnOtherOverlapBegin);
}

void ARangedEnemyProjectile::Tick(float pDeltaTime)
{
	Super::Tick(pDeltaTime);

	SetActorLocation(GetActorLocation() + GetActorForwardVector() * TravelSpeed * pDeltaTime);
	lifeSpawn -= pDeltaTime;
	if(lifeSpawn <= 0)
		Destroy();
}

void ARangedEnemyProjectile::OnOtherOverlapBegin(UPrimitiveComponent *OverlappedComponent, AActor *OtherActor, UPrimitiveComponent *OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult)
{
	APlayerPaperCharacter *player = Cast<APlayerPaperCharacter>(OtherActor);
	if(!player) return;

	player->TakeDamage(Damage, GetActorForwardVector(), Knockback);
	Destroy();
}
