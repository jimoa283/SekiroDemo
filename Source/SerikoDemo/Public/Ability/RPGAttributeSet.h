// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "Net/UnrealNetwork.h"
#include "AbilitySystemComponent.h"
#include "RPGAttributeSet.generated.h"

// Uses marco from AttributeSet.h
#define ATTRIBUTE_ACCESSORS(ClassName,PropertyName) \
    GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName,PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
    GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

/**
 * 
 */
UCLASS()
class SERIKODEMO_API URPGAttributeSet : public UAttributeSet
{
	GENERATED_BODY()

public:
    
	URPGAttributeSet();

	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	virtual void PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data) override;
	virtual void GetLifetimeReplicatedProps( TArray<class FLifetimeProperty> & OutLifetimeProps ) const override;

	UPROPERTY(BlueprintReadOnly,Category="Health",ReplicatedUsing=OnRep_Health)
	FGameplayAttributeData Health;
	ATTRIBUTE_ACCESSORS(URPGAttributeSet,Health)

	UPROPERTY(BlueprintReadOnly,Category="Health",ReplicatedUsing=OnRep_MaxHealth)
	FGameplayAttributeData MaxHealth;
	ATTRIBUTE_ACCESSORS(URPGAttributeSet,MaxHealth)

	UPROPERTY(BlueprintReadOnly,Category="Blance",ReplicatedUsing=OnRep_Blance)
	FGameplayAttributeData Blance;
	ATTRIBUTE_ACCESSORS(URPGAttributeSet,Blance)

	UPROPERTY(BlueprintReadOnly,Category="Blance",ReplicatedUsing=OnRep_MaxBlance)
	FGameplayAttributeData MaxBlance;
	ATTRIBUTE_ACCESSORS(URPGAttributeSet,MaxBlance)

	UPROPERTY(BlueprintReadOnly,Category="Damage",ReplicatedUsing=OnRep_AttackPower)
	FGameplayAttributeData AttackPower;
	ATTRIBUTE_ACCESSORS(URPGAttributeSet,AttackPower)

	UPROPERTY(BlueprintReadOnly,Category="MoveSpeed",ReplicatedUsing=OnRep_MoveSpeed)
	FGameplayAttributeData MoveSpeed;
	ATTRIBUTE_ACCESSORS(URPGAttributeSet,MoveSpeed)

	UPROPERTY(BlueprintReadOnly,Category="Damage")
	FGameplayAttributeData Damage;
	ATTRIBUTE_ACCESSORS(URPGAttributeSet,Damage)

	UPROPERTY(BlueprintReadOnly, Category = "BlanceRecoverSpeed", ReplicatedUsing = OnRep_BlanceRecoverSpeed)
	FGameplayAttributeData BlanceRecoverSpeed; 
	ATTRIBUTE_ACCESSORS(URPGAttributeSet, BlanceRecoverSpeed)

protected:
    
	/* Helper function to proportionally adjust the value of an attribute when it's associated max attribute changes. (i.e. When MaxHealth increases, Health increases by an amount that maintains the same percentage as before) */
    void AdjustAttributeForMaxChange(FGameplayAttributeData& AffectedAttribute,const FGameplayAttributeData& MaxAttribute,float NewMaxValue,const FGameplayAttribute& AffectedAttributeProperty);

    // These OnRep fucntions exist to make sure that the ability system internal representations are synchronized properly during replication
	UFUNCTION()
	virtual void OnRep_Health(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	virtual void OnRep_MaxHealth(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	virtual void OnRep_Blance(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	virtual void OnRep_MaxBlance(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	virtual void OnRep_AttackPower(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	virtual void OnRep_MoveSpeed(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	virtual void OnRep_BlanceRecoverSpeed(const FGameplayAttributeData& OldValue);

};
