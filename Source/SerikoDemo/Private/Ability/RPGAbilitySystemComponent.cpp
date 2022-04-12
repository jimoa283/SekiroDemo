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
    /* FGameplayAbilitySpec �Ƕ�GameplayAbility�İ�װ,��������Ability����ʱ��Ҫ����Ϣ */
    TArray<FGameplayAbilitySpec*> AbiltiesToActive;

    /* UAbilitySystemComponent�ķ���,������пɼ����Ability Specification */
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