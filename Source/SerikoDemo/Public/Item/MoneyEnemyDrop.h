// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MoneyEnemyDrop.generated.h"

class USphereComponent;
class UParticleSystemComponent;



UCLASS()
class SERIKODEMO_API AMoneyEnemyDrop : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMoneyEnemyDrop();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category=Components)
	USphereComponent* Sphere;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category=Component)
	UParticleSystemComponent* Summon;

	UPROPERTY(BlueprintReadWrite)
    int32 Money;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void Init(int32 Value);

};
