// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/RPGAssetManager.h"
#include "Engine/DataAsset.h"
#include "Styling/SlateBrush.h"
#include "RPGItem.generated.h"

class URPGGameplayAbility;

/**
 * 
 */
UCLASS()
class SERIKODEMO_API URPGItem : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
    
	URPGItem()
	  : Price(0)
	  , MaxCount(1)
	  , MaxLevel(1)
	  , AbilityLevel(1)
	{}
    
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category=Item)
	FPrimaryAssetType ItemType;

	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category=Item)
	FText ItemName;

	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category=Item)
	FText ItemDescription;

	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category=Item)
	FSlateBrush ItemIcon;

	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category=Item)
	int32 Price;

	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category=Item)
	int32 MaxCount;

	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category=Item)
	int32 MaxLevel;

	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category=Ability)
	TSubclassOf<URPGGameplayAbility> ItemAbility;

	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category=Ability)
	int32 AbilityLevel;

	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category=Ability)
	bool bCantBeMove;

	/* Returns the logical name, equivalent to the primary asset id */
	UFUNCTION(BlueprintCallable,Category=Item)
	FString GetIdentifierString() const;

	/* Overridden to use saved type */
	virtual FPrimaryAssetId GetPrimaryAssetId() const override;
	
};
