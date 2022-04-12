// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectExecutionCalculation.h"
#include "BlanceSmoothDownExecution.generated.h"

/**
 * 
 */
UCLASS()
class SERIKODEMO_API UBlanceSmoothDownExecution : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()

public:

    UBlanceSmoothDownExecution();

	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;

};
