// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "Ability/RPGAbilityTypes.h"
#include "RPGGameplayAbility.generated.h"

/**
*  Subclass of ability blueprint type with game-specific data
*  This class uses GameplayEffectContainers to allow execution of gameplay effects based on a triggering tag
 * Most games will need to implement a subclass to support their game-specific code
 */
UCLASS()
class SERIKODEMO_API URPGGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()

public:
    
	URPGGameplayAbility();

	/* Map if gameplay tags to gameplay effect containers */
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category=GameplayEffect)
	TMap<FGameplayTag,FRPGameplayEffectContainer> EffectContainerMap;
	
	/* Make gameplay effect container spec to be applied later, using the passed in container */
	UFUNCTION(BlueprintCallable,Category=Ability,meta=(AutoCreateRefTerm="EventData"))
    virtual FRPGGameplayEffectContainerSpec MakeEffectContainerSpecFromContainer(const FRPGameplayEffectContainer& Container,const FGameplayEventData& EventData,int32 OverrideGameplayLevel=-1);

	/* Search for and make a gameplay effect container spec to be applied later, from the EffectContainerMap */
	UFUNCTION(BlueprintCallable,Category=Ability,meta=(AutoCreateRefTerm="EventData"))
	virtual FRPGGameplayEffectContainerSpec MakeEffectContainerSpec(FGameplayTag ContainerTag,const FGameplayEventData& EventData,int32 OverrideGameplayLevel=-1);

	/* Applies a gameplay effect container spec that was previously created */
	UFUNCTION(BlueprintCallable,Category=Ability)
	virtual TArray<FActiveGameplayEffectHandle> ApplyEffectContainerSpec(const FRPGGameplayEffectContainerSpec& ContainerSpec);

	/* Applies a gameplay effect container, by creating and then applying the spec */
	UFUNCTION(BlueprintCallable,Category=Ability,meta=(AutoCreateRefTerm="EventData"))
	virtual TArray<FActiveGameplayEffectHandle> ApplyEffectContainer(FGameplayTag ContainerTag,const FGameplayEventData& EventData,int32 OverrideGameplayLevel=-1);

};
