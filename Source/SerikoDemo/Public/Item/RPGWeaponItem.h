// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/RPGItem.h"
#include "RPGWeaponItem.generated.h"

/**
 * 
 */
UCLASS()
class SERIKODEMO_API URPGWeaponItem : public URPGItem
{
	GENERATED_BODY()

public:
    
	URPGWeaponItem()
	{
		ItemType=URPGAssetManager::WeaponItemType;
	}

	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category=Weapon)
	TSubclassOf<AActor> WeaponActor;
	
};
