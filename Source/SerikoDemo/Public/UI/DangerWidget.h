// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DangerWidget.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnUpdateCompleted);

/**
 * 
 */
UCLASS()
class SERIKODEMO_API UDangerWidget : public UUserWidget
{
	GENERATED_BODY()

public:

    UFUNCTION(BlueprintImplementableEvent)
    void UpdateImage();
	
	UPROPERTY(BlueprintCallable,BlueprintAssignable)
	FOnUpdateCompleted OnUpdateCompleted;
};
