// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/RPGItem.h"
#include "RPGTokenItem.generated.h"

/**
 * 
 */
UCLASS()
class SERIKODEMO_API URPGTokenItem : public URPGItem
{
	GENERATED_BODY()
	
public:
    
	URPGTokenItem()
	{
		ItemType=URPGAssetManager::TokenItemType;
		MaxCount=0; // Infinite
	}

};
