// Fill out your copyright notice in the Description page of Project Settings.


#include "Ability/RPGAttributeSet.h"
#include "Ability/RPGAbilitySystemComponent.h"
#include "Character/BaseCharacter.h"
#include "GameplayEffect.h"
#include "GameplayEffectExtension.h"


URPGAttributeSet::URPGAttributeSet()
       : Health(1.f)
	   , MaxHealth(1.f)
	   , Blance(0.f)
	   , MaxBlance(1.f)
	   , AttackPower(1.f)
	   , MoveSpeed(1.f)
	   , Damage(0.0f)
	   , BlanceRecoverSpeed(0.0f)
{
   
}

void URPGAttributeSet::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(URPGAttributeSet, Health);
	DOREPLIFETIME(URPGAttributeSet, MaxHealth);
	DOREPLIFETIME(URPGAttributeSet, Blance);
	DOREPLIFETIME(URPGAttributeSet, MaxBlance);
	DOREPLIFETIME(URPGAttributeSet, AttackPower);
	DOREPLIFETIME(URPGAttributeSet, MoveSpeed);
	DOREPLIFETIME(URPGAttributeSet,BlanceRecoverSpeed);
}

void URPGAttributeSet::OnRep_Health(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(URPGAttributeSet, Health, OldValue);
}

void URPGAttributeSet::OnRep_MaxHealth(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(URPGAttributeSet, MaxHealth, OldValue);
}

void URPGAttributeSet::OnRep_Blance(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(URPGAttributeSet, Blance, OldValue);
}

void URPGAttributeSet::OnRep_MaxBlance(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(URPGAttributeSet, MaxBlance, OldValue);
}

void URPGAttributeSet::OnRep_AttackPower(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(URPGAttributeSet, AttackPower, OldValue);
}

void URPGAttributeSet::OnRep_MoveSpeed(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(URPGAttributeSet, MoveSpeed, OldValue);
}

void URPGAttributeSet::OnRep_BlanceRecoverSpeed(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(URPGAttributeSet, BlanceRecoverSpeed, OldValue);
}

void URPGAttributeSet::AdjustAttributeForMaxChange(FGameplayAttributeData& AffectedAttribute, const FGameplayAttributeData& MaxAttribute, float NewMaxValue, const FGameplayAttribute& AffectedAttributeProperty)
{
	//获得AbilitySystemComponent实例
	UAbilitySystemComponent* AbilityComp = GetOwningAbilitySystemComponent();

	//Getter方法获取的都是current value而不是base value
	const float CurrentMaxValue = MaxAttribute.GetCurrentValue();

	if (!FMath::IsNearlyEqual(CurrentMaxValue, NewMaxValue) && AbilityComp)
	{
		// Change current value to maintain the current Value/ Max percent
		const float CurrentValue = AffectedAttribute.GetCurrentValue();
		float NewDelta = (CurrentMaxValue > 0.f) ? (CurrentValue * NewMaxValue / CurrentMaxValue) - CurrentValue : NewMaxValue;

		// 通过AbilitySystemComponent实例
		AbilityComp->ApplyModToAttributeUnsafe(AffectedAttributeProperty, EGameplayModOp::Additive, NewDelta);

	}
}

void URPGAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
    // This is called whenever attributes change, so  for max health/blance we want to scale the current totals to match
	Super::PreAttributeChange(Attribute, NewValue);
	
	if (Attribute == GetMaxHealthAttribute())
	{
		AdjustAttributeForMaxChange(Health,MaxHealth,NewValue,GetHealthAttribute());
	}
	else if (Attribute == GetMaxBlanceAttribute())
	{
		AdjustAttributeForMaxChange(Blance,MaxBlance,NewValue,GetBlanceAttribute());
	}
}

void URPGAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);
	
	FGameplayEffectContextHandle Context = Data.EffectSpec.GetContext();
	UAbilitySystemComponent* Source=Context.GetOriginalInstigatorAbilitySystemComponent();
	const FGameplayTagContainer& SourceTags=*Data.EffectSpec.CapturedSourceTags.GetAggregatedTags();

	// Compute the delta between old and new,if it is available
	float DeltaValue=0;
	if (Data.EvaluatedData.ModifierOp == EGameplayModOp::Type::Additive)
	{
		// If this was additive,store the raw delta value to be passed along later
		DeltaValue=Data.EvaluatedData.Magnitude;
	}

	// Get the Target actor, which should be our owner
	AActor* TargetActor=nullptr;
	AController* TargetController=nullptr;
	ABaseCharacter* TargetCharacter=nullptr;

	if (Data.Target.AbilityActorInfo.IsValid() && Data.Target.AbilityActorInfo->AvatarActor.IsValid())
	{
		TargetActor=Data.Target.AbilityActorInfo->AvatarActor.Get();
		TargetController=Data.Target.AbilityActorInfo->PlayerController.Get();
		TargetCharacter=Cast<ABaseCharacter>(TargetActor);
	}

	if (Data.EvaluatedData.Attribute == GetDamageAttribute())
	{

	   // Get the Source actor
	   AActor* SourceActor=nullptr;
	   AController* SourceController=nullptr;
	   ABaseCharacter* SourceCharacter=nullptr;
	   if (Source && Source->AbilityActorInfo.IsValid() && Source->AbilityActorInfo->AvatarActor.IsValid())
	   {
		   SourceActor=Source->AbilityActorInfo->AvatarActor.Get();
		   SourceController=Source->AbilityActorInfo->PlayerController.Get();
		   if (SourceController == nullptr && SourceActor != nullptr)
		   {
			   if (APawn* Pawn = Cast<APawn>(SourceActor))
			   {
				   SourceController=Pawn->GetController();
			   }
		   }

		   // Use the controller to find the source pawn
		   if (SourceController)
		   {
			   SourceCharacter=Cast<ABaseCharacter>(SourceController->GetPawn());
		   }
		   else
		   {
			   SourceCharacter=Cast<ABaseCharacter>(SourceActor);
		   }

		   // Set the Causer actor based on context if it's set
		   if (Context.GetEffectCauser())
		   {
			   SourceActor=Context.GetEffectCauser();
		   }
	   }

	   // Try to extract a hit result 
	   FHitResult HitResult;
	   if (Context.GetHitResult())
	   {
		   HitResult=*Context.GetHitResult();
	   }

	   // Store a local copy of the amount of damage done and  clear the damage attribute
	   const float LocalDamageDone=GetDamage();
	   SetDamage(0.f);
	    
	   if (LocalDamageDone > 0)
	   {
		   //Apply the health change and then clamp it
		   const float OldHealth=GetHealth();
		   SetHealth(FMath::Clamp(OldHealth-LocalDamageDone,0.0f,GetMaxHealth()));

		   if (TargetCharacter)
		   {
			   // This is proper damage
			   if(TargetCharacter->GetCharacterState() != ECharacterState::LoseBlance)
			     TargetCharacter->HandleDamage(LocalDamageDone, HitResult, SourceTags, SourceCharacter, SourceActor);

			   // Call for all health changes
			   TargetCharacter->HandleHealthChanged(-LocalDamageDone, SourceTags);
		   }
		}
	}
	else if (Data.EvaluatedData.Attribute == GetHealthAttribute())
	{
		//Handle other health changes such as from healing or direct modifiers
        // First clamp it 
		SetHealth(FMath::Clamp(GetHealth(),0.0f,GetMaxHealth()));

		if (TargetCharacter)
		{
			// Call for all health changes
			TargetCharacter->HandleHealthChanged(DeltaValue,SourceTags);
		}
	}
	else if (Data.EvaluatedData.Attribute == GetBlanceAttribute())
	{
		SetBlance(FMath::Clamp(GetBlance(),0.0f,GetMaxBlance()));

		if (TargetCharacter&&TargetCharacter->GetCharacterState()!=ECharacterState::LoseBlance)
		{
			TargetCharacter->HandleBlanceChanged(DeltaValue,SourceTags);
		}
	}
	else if (Data.EvaluatedData.Attribute == GetMoveSpeedAttribute())
	{
		if (TargetCharacter)
		{
			TargetCharacter->HandleMoveSpeedChanged(DeltaValue,SourceTags);
		}
	}
}







