// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/RPGTypes.h"
#include "Ability/RPGAbilityTypes.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "RPGFunctionLibrary.generated.h"

/**
 * 
 */
UCLASS()
class SERIKODEMO_API URPGFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
    
	/* Equality operator for ItemSlot */
	UFUNCTION(BlueprintPure,meta=(DisplayName="Equal(RPGItemSlot)",CompactNodeTitle="==",Keywords="== equal"),Category=Inventory)
	static bool EqualEqual_RPGItemSlot(const FRPGItemSlot& A,const FRPGItemSlot& B);

	/* Inequality operator for ItemSlot */
	UFUNCTION(BlueprintPure,meta=(DisplayName="NotEqualEqual(RPGItemSlot)",CompactNodeTitle="!=",Keywords="!= not equal"),Category=Inventory)
	static bool NotEqual_RPGItemSlot(const FRPGItemSlot& A,const FRPGItemSlot& B);

	/* Validity check for ItemSlot */
	UFUNCTION(BlueprintPure,Category=Inventory)
	static bool IsValidItemSlot(const FRPGItemSlot& ItemSlot);

	/* Checks if spec has any effects */
	UFUNCTION(BlueprintPure,Category=Ability)
	static bool DoesEffectContainerSpecHaveEffects(const FRPGGameplayEffectContainerSpec& ContainerSpec);

	/* Checks if spec has any targets */
	UFUNCTION(BlueprintPure,Category=Ability)
	static bool DoesEffectContainerSpecHaveTargets(const FRPGGameplayEffectContainerSpec& ContainerSpec);
	
	/* Add targets to a copy of the passed in effect container spec and returns it */
	UFUNCTION(BlueprintCallable,Category=Ability,meta=(AutoCreateRefTerm="HitResults,TargetActors"))
	static FRPGGameplayEffectContainerSpec AddTargetsToEffectContainerSpec(const FRPGGameplayEffectContainerSpec& ContainerSpec,const TArray<FHitResult>& HitResults,const TArray<AActor*>& TargetActors);

	/* Applies container spec that was made from an ability */
	UFUNCTION(BlueprintCallable,Category=Ability)
	static TArray<FActiveGameplayEffectHandle> ApplyExternalEffectContainerSpec(const FRPGGameplayEffectContainerSpec& ContainerSpec);

};
