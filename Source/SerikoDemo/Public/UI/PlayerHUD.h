// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/CharacterHUD.h"
#include "PlayerHUD.generated.h"

/**
 * 
 */
UCLASS()
class SERIKODEMO_API UPlayerHUD : public UCharacterHUD
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void UpdateMoney(int32 BeforeMoney, int32 DeltaValue);
	
};
