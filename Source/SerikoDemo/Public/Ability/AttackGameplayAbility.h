// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/AttackActionInfo.h"
#include "Ability/RPGGameplayAbility.h"
#include "AttackGameplayAbility.generated.h"

/**
 * 
 */
UCLASS()
class SERIKODEMO_API UAttackGameplayAbility : public URPGGameplayAbility
{
	GENERATED_BODY()

public:
    
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category=AttackInfo)
	UAnimMontage* Montage;
    
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category=AttackInfo)
	TArray<FAttackActionInfo> AttackInfos;
	
};
