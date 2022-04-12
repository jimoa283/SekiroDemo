// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/MoneyEnemyDrop.h"
#include "Components/SphereComponent.h"
#include "Particles\ParticleSystemComponent.h"

// Sets default values
AMoneyEnemyDrop::AMoneyEnemyDrop()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Sphere=CreateDefaultSubobject<USphereComponent>(TEXT("Sphere"));
	RootComponent=Sphere;

	Summon=CreateDefaultSubobject<UParticleSystemComponent>(TEXT("Summon"));
	Summon->SetupAttachment(Sphere);
}

// Called when the game starts or when spawned
void AMoneyEnemyDrop::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AMoneyEnemyDrop::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AMoneyEnemyDrop::Init(int32 Value)
{
   Money=Value;
}

