// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "JumpSectionNS.generated.h"

/**
 * 
 */
UCLASS()
class SERIKODEMO_API UJumpSectionNS : public UAnimNotifyState
{
	GENERATED_BODY()

	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration) override;

	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;

public:
    
	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	FName JumpSection;

};
