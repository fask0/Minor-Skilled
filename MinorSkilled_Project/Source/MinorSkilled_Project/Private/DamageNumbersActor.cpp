// Fill out your copyright notice in the Description page of Project Settings.


#include "DamageNumbersActor.h"

// Sets default values
ADamageNumbersActor::ADamageNumbersActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ADamageNumbersActor::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void ADamageNumbersActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FVector actorPosition = GetActorLocation();
	actorPosition.Z += 50 * DeltaTime;
	SetActorLocation(actorPosition);
}

