// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ShowBackEnemyArrow.generated.h"

/**
 * 
 */
UCLASS()
class SERIKODEMO_API UShowBackEnemyArrow : public UUserWidget
{
	GENERATED_BODY()

public:
    
	UFUNCTION(BlueprintImplementableEvent)
	void UpdateArrowAngle(float Angle,bool IsLeft);
	
};
