// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Character/PlayerCharacter.h"
#include "ShowBackEnemyWidget.generated.h"

class ABaseCharacter;
class UShowBackEnemyArrow;

/**
 * 
 */
UCLASS()
class SERIKODEMO_API UShowBackEnemyWidget : public UUserWidget
{
	GENERATED_BODY()

protected:

    UPROPERTY(BlueprintReadWrite,Category=ShowBackEnemy)
    TMap<ABaseCharacter*,UShowBackEnemyArrow*> ArrowMap;

public:
    
	UFUNCTION(BlueprintImplementableEvent)
	void UpdateShowBackEnemyArrow(const TMap<ABaseCharacter*,FShowBackEnemyInfo>& ShowBackEnemyInfoMap);

	UFUNCTION(BlueprintImplementableEvent)
	void AddShowBackEnemyInfo(ABaseCharacter* Enemy);

	UFUNCTION(BlueprintImplementableEvent)
	void RemoveShowBackEnemyInfo(ABaseCharacter* Enemy);
	
};
