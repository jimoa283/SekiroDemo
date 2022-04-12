// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Ability/RPGGameplayAbility.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "WeaponAttackNS.generated.h"

/**
 * 
 */
UCLASS()
class SERIKODEMO_API UWeaponAttackNS : public UAnimNotifyState
{
	GENERATED_BODY()
	
    virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration) override;

	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;

public: 

    UPROPERTY(EditAnywhere)
	FGameplayTag EventTag;

	UPROPERTY(EditAnywhere)
	int32 OpenTraceCount;
};