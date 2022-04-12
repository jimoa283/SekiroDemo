// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectExecutionCalculation.h"
#include "AttackHitExecution.generated.h"

/**
 * 
 */
UCLASS()
class SERIKODEMO_API UAttackHitExecution : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()
	
public:
    UAttackHitExecution();
	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;

};
