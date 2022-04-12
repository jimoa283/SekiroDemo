// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DetectionNode.generated.h"

/**
 * 
 */
UCLASS()
class SERIKODEMO_API UDetectionNode : public UUserWidget
{
	GENERATED_BODY()

public:

    UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
    void UpdateDistanceCheckImage(float Size);

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void Deativate();

	UFUNCTION(BlueprintImplementableEvent)
	void PlayNodeUseAnimation();
	
};
