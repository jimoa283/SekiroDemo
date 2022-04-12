// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "PlayMontageAndWaitForEvent.generated.h"

class URPGAbilitySystemComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FRPGPlayMontageAndWaitForEventDelegate,FGameplayTag,EventTag,FGameplayEventData,EventData);

/**
 * 
 */
UCLASS()
class SERIKODEMO_API UPlayMontageAndWaitForEvent : public UAbilityTask
{
	GENERATED_BODY()

public: 
    
   UPlayMontageAndWaitForEvent(const FObjectInitializer& ObjectInitializer);
	virtual void Activate() override;
	virtual void ExternalCancel() override;
	virtual FString GetDebugString() const override;
	virtual void OnDestroy(bool AbilityEnded) override;

	/* The montage completely finished playing */
	UPROPERTY(BlueprintAssignable)
	FRPGPlayMontageAndWaitForEventDelegate OnCompleted;

	UPROPERTY(BlueprintAssignable)
	FRPGPlayMontageAndWaitForEventDelegate OnBlendOut;

	UPROPERTY(BlueprintAssignable)
	FRPGPlayMontageAndWaitForEventDelegate OnInterrupted;

	UPROPERTY(BlueprintAssignable)
	FRPGPlayMontageAndWaitForEventDelegate OnCancelled;

	UPROPERTY(BlueprintAssignable)
	FRPGPlayMontageAndWaitForEventDelegate EventReceived;

	UFUNCTION(BlueprintCallable,Category="Ability|Tasks",meta=(HidePin="OwningAbility",DefaultToSelf="OwningAbility",BlueprintInternalUseOnly="TRUE"))
	static UPlayMontageAndWaitForEvent* PlayMontageAndWaitForEvent(
	   UGameplayAbility* OwningAbility,
	   FName TaskInstanceName,
	   UAnimMontage* MontageToPlay,
	   FGameplayTagContainer EventTags,
	   float Rate=1.f,
	   FName StartSection=NAME_None,
	   bool bStopWhenAbilityEnds=true,
	   float AnimRootMotionTranslationScale=1.f
	);

private:
    
	UPROPERTY()
	UAnimMontage* MontageToPlay;

	UPROPERTY()
	FGameplayTagContainer EventTags;

	UPROPERTY()
	float Rate;

	UPROPERTY()
	FName StartSection;

	UPROPERTY()
	float AnimRootMotionTranslationScale;

	UPROPERTY()
	bool bStopWhenAbilityEnds;

	bool StopPlayingMontage();

	URPGAbilitySystemComponent* GetTargetASC();

	void OnMontageBlendingOut(UAnimMontage* Montage,bool bInterrupted);
	void OnAbilityCancelled();
	void OnMontageEnded(UAnimMontage* Montage,bool bInterrupted);
	void OnGameplayEvent(FGameplayTag EventTag,const FGameplayEventData* Payload);

	FOnMontageBlendingOutStarted BlendingOutDelegate;
	FOnMontageEnded MontageEndedDelegate;
	FDelegateHandle CancelledHandle;
	FDelegateHandle EventHandle;

};
