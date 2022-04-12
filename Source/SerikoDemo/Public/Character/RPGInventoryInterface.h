// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Item/RPGTypes.h"
#include "RPGInventoryInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI,meta=(CannotImplementInterfaceInBlueprint))
class URPGInventoryInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class SERIKODEMO_API IRPGInventoryInterface
{
	GENERATED_BODY()

public:
    
	/* Returns the map of items to data */
    //virtual const TMap<URPGItem*,FRPGItemData>& GetInventoryDataMap() const=0;

	/* Returns the map of slots to items */
	//virtual const TMap<FRPGItemSlot,URPGItem*>&  GetSlottedItemMap() const=0;

	/* Gets the delegate for inventory item changes */
	virtual FOnInventoryItemChangedNative& GetInventoryItemChangedDelegate()=0;

	/* Gets the delegate for inventory slot changes */
	virtual FOnSlottedItemChangedNative& GetSlottedItemChangedDelegate()=0;

	/* Gets the delegate for when the inventory loads */
  

};
