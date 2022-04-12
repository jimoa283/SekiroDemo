// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/RPGTypes.h"
#include "Engine/GameInstance.h"
#include "RPGGameInstance.generated.h"

class URPGItem;

/**
 * 
 */
UCLASS()
class SERIKODEMO_API URPGGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
    
   /* List of inventory items to add to new players */
   UPROPERTY(EditAnywhere,BlueprintReadOnly,Category=Inventory)
   TMap<FPrimaryAssetType,FRPGItemData> DefaultUseInventory;

   /* Number of slots for each type of item */
   UPROPERTY(EditAnywhere,BlueprintReadOnly,Category=Inventory)
   TMap<FPrimaryAssetType,int32> ItemSlotsPerType;

   UFUNCTION(BlueprintCallable,Category=Inventory)
   void AddDefaultInventory();


	
};
