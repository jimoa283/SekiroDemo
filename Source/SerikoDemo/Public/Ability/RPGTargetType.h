// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbilityTypes.h"
#include "Ability\RPGAbilityTypes.h"
#include "UObject/NoExportTypes.h"
#include "RPGTargetType.generated.h"

class ABaseCharacter;
class AActor;
struct FGameplayEventData;

/**
 * 
 */
UCLASS(Blueprintable,meta=(ShowWorldContextPin))
class SERIKODEMO_API URPGTargetType : public UObject
{
	GENERATED_BODY()

public: 
    
	URPGTargetType(){}

	UFUNCTION(BlueprintNativeEvent)
	void GetTargets(ABaseCharacter* TargetingCharacter,AActor* TargetingActor,FGameplayEventData EventData,const FGameplayTagContainer& IgnoreTagContainer,TArray<FHitResult>& OutHitResults,TArray<AActor*>& OutActors) const;
	
};

UCLASS(NotBlueprintable)
class SERIKODEMO_API URPGTargetType_UseOwner : public URPGTargetType
{
	GENERATED_BODY()

public:

    URPGTargetType_UseOwner() {}

	virtual void GetTargets_Implementation(ABaseCharacter* TargetingCharacter, AActor* TargetingActor, FGameplayEventData EventData, const FGameplayTagContainer& IgnoreTagContainer,TArray<FHitResult>& OutHitResults, TArray<AActor*>& OutActors) const override;

};

UCLASS(NotBlueprintable)
class SERIKODEMO_API URPGTargetType_UseTarget :public URPGTargetType
{
	GENERATED_BODY()

public:
    
	URPGTargetType_UseTarget(){}

	virtual void GetTargets_Implementation(ABaseCharacter* TargetingCharacter, AActor* TargetingActor, FGameplayEventData EventData, const FGameplayTagContainer& IgnoreTagContainer,TArray<FHitResult>& OutHitResults, TArray<AActor*>& OutActors) const override;

};

UCLASS(NotBlueprintable)
class SERIKODEMO_API URPGTargetType_UseEventData : public URPGTargetType
{
	GENERATED_BODY()

public:

    URPGTargetType_UseEventData(){}

	virtual void GetTargets_Implementation(ABaseCharacter* TargetingCharacter, AActor* TargetingActor, FGameplayEventData EventData, const FGameplayTagContainer& IgnoreTagContainer,TArray<FHitResult>& OutHitResults, TArray<AActor*>& OutActors) const override;

};