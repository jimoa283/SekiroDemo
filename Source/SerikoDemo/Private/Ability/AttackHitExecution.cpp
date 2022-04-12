// Fill out your copyright notice in the Description page of Project Settings.


#include "Ability/AttackHitExecution.h"
#include "Character/BaseCharacter.h"

struct AttackHitStatics
{
   DECLARE_ATTRIBUTE_CAPTUREDEF(AttackPower);
   DECLARE_ATTRIBUTE_CAPTUREDEF(Damage);

   AttackHitStatics()
   {
	   DEFINE_ATTRIBUTE_CAPTUREDEF(URPGAttributeSet,AttackPower,Source,true);
	   DEFINE_ATTRIBUTE_CAPTUREDEF(URPGAttributeSet,Damage,Source,true);
   }
};

static const AttackHitStatics& HitStatics()
{
	static AttackHitStatics ATHStatics;
	return ATHStatics;
}

UAttackHitExecution::UAttackHitExecution()
{
  RelevantAttributesToCapture.Add(HitStatics().AttackPowerDef);
  RelevantAttributesToCapture.Add(HitStatics().DamageDef);
}

void UAttackHitExecution::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	UAbilitySystemComponent* TargetAbilitySystemComponent=ExecutionParams.GetTargetAbilitySystemComponent();
	UAbilitySystemComponent* SourceAbilitySystemComponent=ExecutionParams.GetSourceAbilitySystemComponent();

	AActor* SoureActor=SourceAbilitySystemComponent?SourceAbilitySystemComponent->GetAvatarActor_Direct():nullptr;
	AActor* TargetActor=TargetAbilitySystemComponent?TargetAbilitySystemComponent->GetAvatarActor_Direct():nullptr;

	const FGameplayEffectSpec& Spec=ExecutionParams.GetOwningSpec();

	const FGameplayTagContainer* SourceTags=Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags=Spec.CapturedTargetTags.GetAggregatedTags();

	FAggregatorEvaluateParameters EvaluationParameters;
	EvaluationParameters.SourceTags=SourceTags;
	EvaluationParameters.TargetTags=TargetTags;

	float AttackPower=0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(HitStatics().AttackPowerDef,EvaluationParameters,AttackPower);

	float Damage=1.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(HitStatics().DamageDef,EvaluationParameters,Damage);

	float DamageDone=AttackPower;
	if (ABaseCharacter* SourceCharacter = Cast<ABaseCharacter>(SoureActor))
	{
		DamageDone*=SourceCharacter->GetCurrentAttackInfo().BaseHealthDamage;
	}

	if (DamageDone > 0.f)
	{
		OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(HitStatics().DamageProperty, EGameplayModOp::Additive, DamageDone));
	}
    
}
