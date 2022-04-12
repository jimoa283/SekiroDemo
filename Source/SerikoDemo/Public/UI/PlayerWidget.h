// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerWidget.generated.h"

/**
 * 
 */
UCLASS()
class SERIKODEMO_API UPlayerWidget : public UUserWidget
{
	GENERATED_BODY()

public:
    
	UFUNCTION(BlueprintCallable,BlueprintImplementableEvent)
	void UpdateMoney(int32 BeforeMoney,int32 DeltaValue);
	
};
