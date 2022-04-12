// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CharacterHUD.generated.h"

/**
 * 
 */
UCLASS()
class SERIKODEMO_API UCharacterHUD : public UUserWidget
{
	GENERATED_BODY()
	
public: 
    
	UFUNCTION(BlueprintImplementableEvent)
	void UpdateHealthBar(float DeltaValue,float CurrentValue,float MaxValue);

	UFUNCTION(BlueprintImplementableEvent)
	void UpdateBlanceBar(float CurrentValue,float MaxValue);

};
