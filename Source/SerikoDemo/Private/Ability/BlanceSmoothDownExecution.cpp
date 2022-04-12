// Fill out your copyright notice in the Description page of Project Settings.


#include "Ability/BlanceSmoothDownExecution.h"
#include "Character/BasePlayerCharacter.h"

struct BlanceSmoothDownStatics
{
	DECLARE_ATTRIBUTE_CAPTUREDEF(MaxHealth);
	DECLARE_ATTRIBUTE_CAPTUREDEF(Health);
	DECLARE_ATTRIBUTE_CAPTUREDEF(BlanceRecoverSpeed);
	DECLARE_ATTRIBUTE_CAPTUREDEF(Blance);

	BlanceSmoothDownStatics()
	{
		DEFINE_ATTRIBUTE_CAPTUREDEF(URPGAttributeSet,MaxHealth,Source,true);
		DEFINE_ATTRIBUTE_CAPTUREDEF(URPGAttributeSet,Health,Source,true);
		DEFINE_ATTRIBUTE_CAPTUREDEF(URPGAttributeSet,BlanceRecoverSpeed,Source,true);
		DEFINE_ATTRIBUTE_CAPTUREDEF(URPGAttributeSet,Blance,Source,true);
	}
};

static const BlanceSmoothDownStatics& BlanceRecoverStatics()
{
	static BlanceSmoothDownStatics BSDStatic;
	return BSDStatic;
}

UBlanceSmoothDownExecution::UBlanceSmoothDownExecution()
{
	RelevantAttributesToCapture.Add(BlanceRecoverStatics().MaxHealthDef);
	RelevantAttributesToCapture.Add(BlanceRecoverStatics().HealthDef);
	RelevantAttributesToCapture.Add(BlanceRecoverStatics().BlanceRecoverSpeedDef);
	RelevantAttributesToCapture.Add(BlanceRecoverStatics().BlanceDef);
}

void UBlanceSmoothDownExecution::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
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

	float Health=0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(BlanceRecoverStatics().HealthDef,EvaluationParameters,Health);

	float MaxHealth=0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(BlanceRecoverStatics().MaxHealthDef,EvaluationParameters,MaxHealth);

	float BlanceRecoverSpeed=0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(BlanceRecoverStatics().BlanceRecoverSpeedDef,EvaluationParameters,BlanceRecoverSpeed);

	float Blance=0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(BlanceRecoverStatics().BlanceDef,EvaluationParameters,Blance);

	float HealthRatio=Health/MaxHealth;
	float RecoverRate;
	if(HealthRatio>0.75f)
	   RecoverRate=1.0f;
	else if(HealthRatio>0.5f)
	  RecoverRate=0.75f;
	else if(HealthRatio>0.25)
	  RecoverRate=0.5f;
	else
	  RecoverRate=0.25f;

	float BlanceDone=-RecoverRate*BlanceRecoverSpeed*0.05f;

	OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(BlanceRecoverStatics().BlanceProperty,EGameplayModOp::Additive,BlanceDone));
}
