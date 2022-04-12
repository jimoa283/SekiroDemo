// Fill out your copyright notice in the Description page of Project Settings.


#include "Ability/RPGAbilitySystemComponent.h"
#include "Character/BaseCharacter.h"
#include "Ability\RPGGameplayAbility.h"
#include "AbilitySystemGlobals.h"

URPGAbilitySystemComponent::URPGAbilitySystemComponent()
{

}

void URPGAbilitySystemComponent::GetActiveAbilitiesWithTags(const FGameplayTagContainer& GameplayTagContainer, TArray<URPGGameplayAbility*>& ActiveAbilities)
{
    /* FGameplayAbilitySpec 是对GameplayAbility的包装,还包含了Ability运行时必要的信息 */
    TArray<FGameplayAbilitySpec*> AbiltiesToActive;

    /* UAbilitySystemComponent的方法,获得所有可激活的Ability Specification */
    GetActivatableGameplayAbilitySpecsByAllMatchingTags(GameplayTagContainer,AbiltiesToActive,false);

    for (FGameplayAbilitySpec* Spec : AbiltiesToActive)
    {
       TArray<UGameplayAbility*> AbilityInstances=Spec->GetAbilityInstances();
       
       for (UGameplayAbility* ActiveAbility : AbilityInstances)
       {
           ActiveAbilities.Add(Cast<URPGGameplayAbility>(ActiveAbility));
       }
    }

}

int32 URPGAbilitySystemComponent::GetDefaultAbilityLevel() const
{
    ABaseCharacter* OwningCharacter =Cast<ABaseCharacter>(GetOwner());

    if (OwningCharacter)
    {
        return 1;
    }
    return 1;
}

URPGAbilitySystemComponent* URPGAbilitySystemComponent::GetAbilitySystemComponentFromActor(const AActor* Actor, bool LookForComponent/*=false*/)
{
   return Cast<URPGAbilitySystemComponent>(UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Actor,LookForComponent));
}
