// Fill out your copyright notice in the Description page of Project Settings.


#include "Ability/DefendHitExecution.h"
#include "Character/BaseCharacter.h"

struct DefendHitStatics
{
	DECLARE_ATTRIBUTE_CAPTUREDEF(Blance);

	DefendHitStatics()
	{
		DEFINE_ATTRIBUTE_CAPTUREDEF(URPGAttributeSet,Blance,Target,false);
	}
};

static const DefendHitStatics& DHitStatics()
{
	static DefendHitStatics DFHStatics;
	return DFHStatics;
}


UDefendHitExecution::UDefendHitExecution()
{
	RelevantAttributesToCapture.Add(DHitStatics().BlanceDef);
}

void UDefendHitExecution::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	UAbilitySystemComponent* TargetAbilitySystemComponent = ExecutionParams.GetTargetAbilitySystemComponent();
	UAbilitySystemComponent* SourceAbilitySystemComponent = ExecutionParams.GetSourceAbilitySystemComponent();

	AActor* SoureActor = SourceAbilitySystemComponent ? SourceAbilitySystemComponent->GetAvatarActor_Direct() : nullptr;
	AActor* TargetActor = TargetAbilitySystemComponent ? TargetAbilitySystemComponent->GetAvatarActor_Direct() : nullptr;

	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();

	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	FAggregatorEvaluateParameters EvaluationParameters;
	EvaluationParameters.SourceTags = SourceTags;
	EvaluationParameters.TargetTags = TargetTags;

	float BlanceDamage=1.f;
	if (ABaseCharacter* SourceCharacter=Cast<ABaseCharacter>(SoureActor))
	{
		BlanceDamage*=SourceCharacter->GetCurrentAttackInfo().BaseBlanceDamage;
	}

	OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(DHitStatics().BlanceProperty,EGameplayModOp::Additive,BlanceDamage));
}
