// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/RPGItem.h"
#include "RPGSkillItem.generated.h"

/**
 * 
 */
UCLASS()
class SERIKODEMO_API URPGSkillItem : public URPGItem
{
	GENERATED_BODY()

public:

	URPGSkillItem()
	{
		ItemType=URPGAssetManager::SkillItemType;
	}

	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category=Skill)
	int32 SkillCost;

	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category=Skill)
	FSlateBrush SkillDemonstration;
	
};
