// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectExecutionCalculation.h"
#include "DefendHitExecution.generated.h"

/**
 * 
 */
UCLASS()
class SERIKODEMO_API UDefendHitExecution : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()

public:
 
    UDefendHitExecution();
	
	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;

};
