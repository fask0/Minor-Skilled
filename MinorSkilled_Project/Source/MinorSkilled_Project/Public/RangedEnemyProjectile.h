// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PaperCharacter.h"
#include "RangedEnemyProjectile.generated.h"

/**
 *
 */
UCLASS()
class MINORSKILLED_PROJECT_API ARangedEnemyProjectile : public APaperCharacter
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;
	virtual void Tick(float pDeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variable")
		FVector PlayerLocation;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variable")
		int Damage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variable")
		float Knockback;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variable")
		bool isTickEnabled = true;

	UFUNCTION()
		void OnOtherOverlapBegin(UPrimitiveComponent *OverlappedComponent,
								 AActor *OtherActor,
								 UPrimitiveComponent *OtherComp,
								 int32 OtherBodyIndex,
								 bool bFromSweep,
								 const FHitResult &SweepResult);

protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement)
		float TravelSpeed;

	float lifeSpawn = 5;
};
