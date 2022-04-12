// Fill out your copyright notice in the Description page of Project Settings.


#include "Ability/RPGTargetType.h"
#include "Character/BaseCharacter.h"

void URPGTargetType::GetTargets_Implementation(ABaseCharacter* TargetingCharacter, AActor* TargetingActor, FGameplayEventData EventData, const FGameplayTagContainer& IgnoreTagContainer,TArray<FHitResult>& OutHitResults, TArray<AActor*>& OutActors) const
{ 
     return;
}

void URPGTargetType_UseOwner::GetTargets_Implementation(ABaseCharacter* TargetingCharacter, AActor* TargetingActor, FGameplayEventData EventData, const FGameplayTagContainer& IgnoreTagContainer,TArray<FHitResult>& OutHitResults, TArray<AActor*>& OutActors) const
{
	OutActors.Add(TargetingActor);
}

void URPGTargetType_UseEventData::GetTargets_Implementation(ABaseCharacter* TargetingCharacter, AActor* TargetingActor, FGameplayEventData EventData, const FGameplayTagContainer& IgnoreTagContainer,TArray<FHitResult>& OutHitResults, TArray<AActor*>& OutActors) const
{
	const FHitResult* FoundHitResult=EventData.ContextHandle.GetHitResult();
	if (FoundHitResult)
	{
		if (ABaseCharacter* Target = Cast<ABaseCharacter>(FoundHitResult->GetActor()))
		{
			 FGameplayTagContainer Temp;
			 Target->GetAbilitySystemComponent()->GetOwnedGameplayTags(Temp);

			 if(!Temp.HasAny(IgnoreTagContainer))
				 OutHitResults.Add(*FoundHitResult);
		}
		else 
		   OutHitResults.Add(*FoundHitResult);
	}
	else if (EventData.Target)
	{
	    AActor* TempActor= const_cast<AActor*>(EventData.Target);
		if (ABaseCharacter* Target = Cast<ABaseCharacter>(TempActor))
		{
			FGameplayTagContainer Temp;
			Target->GetAbilitySystemComponent()->GetOwnedGameplayTags(Temp);

			if (!Temp.HasAny(IgnoreTagContainer))
				OutActors.Add(TempActor);
		}
		else
		   OutActors.Add(TempActor);
	}
}

void URPGTargetType_UseTarget::GetTargets_Implementation(ABaseCharacter* TargetingCharacter, AActor* TargetingActor, FGameplayEventData EventData, const FGameplayTagContainer& IgnoreTagContainer,TArray<FHitResult>& OutHitResults, TArray<AActor*>& OutActors) const
{
	if (EventData.Target)
	{
		OutActors.Add(const_cast<AActor*>(EventData.Target));
	}	  

}
