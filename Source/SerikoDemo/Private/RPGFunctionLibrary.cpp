// Fill out your copyright notice in the Description page of Project Settings.


#include "RPGFunctionLibrary.h"

bool URPGFunctionLibrary::EqualEqual_RPGItemSlot(const FRPGItemSlot& A, const FRPGItemSlot& B)
{
   return A==B;
}

bool URPGFunctionLibrary::NotEqual_RPGItemSlot(const FRPGItemSlot& A, const FRPGItemSlot& B)
{
  return A!=B;
}

bool URPGFunctionLibrary::IsValidItemSlot(const FRPGItemSlot& ItemSlot)
{
   return  ItemSlot.IsVaild();
}

bool URPGFunctionLibrary::DoesEffectContainerSpecHaveEffects(const FRPGGameplayEffectContainerSpec& ContainerSpec)
{
   return ContainerSpec.HasValidEffects();
}

bool URPGFunctionLibrary::DoesEffectContainerSpecHaveTargets(const FRPGGameplayEffectContainerSpec& ContainerSpec)
{
   return ContainerSpec.HasValidTargets();
}

FRPGGameplayEffectContainerSpec URPGFunctionLibrary::AddTargetsToEffectContainerSpec(const FRPGGameplayEffectContainerSpec& ContainerSpec, const TArray<FHitResult>& HitResults, const TArray<AActor*>& TargetActors)
{
   FRPGGameplayEffectContainerSpec NewSpec=ContainerSpec;
   NewSpec.AddTargets(HitResults,TargetActors);
   return NewSpec;
}

TArray<FActiveGameplayEffectHandle> URPGFunctionLibrary::ApplyExternalEffectContainerSpec(const FRPGGameplayEffectContainerSpec& ContainerSpec)
{
   TArray<FActiveGameplayEffectHandle> AllEffects;

   //Iterate list of gameplay effects
   for (const FGameplayEffectSpecHandle& SpecHandle : ContainerSpec.TargetGameplayEffectSpecs)
   {
       //If effect if valid, iterate list of targets and apply to all
       for (TSharedPtr<FGameplayAbilityTargetData> Data : ContainerSpec.TargetData.Data)
       {
           AllEffects.Append(Data->ApplyGameplayEffectSpec(*SpecHandle.Data.Get()));
       }
   }

   return AllEffects;
}
