// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/BaseCharacter.h"
#include "AbilitySystemGlobals.h"
#include "Ability/RPGGameplayAbility.h"
#include "AnimNotify/JumpSectionNS.h"
#include "GameFramework\CharacterMovementComponent.h"
#include "Item/RPGItem.h"
#include "Character/InventoryComponent.h"
#include "UI/CharacterHUD.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
ABaseCharacter::ABaseCharacter()
{
   AbilitySystemComponent=CreateDefaultSubobject<URPGAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
   AbilitySystemComponent->SetIsReplicated(true);
 
   AttributeSet=CreateDefaultSubobject<URPGAttributeSet>(TEXT("AttributeSet"));

   //Inventory=CreateDefaultSubobject<UInventoryComponent>(TEXT("Inventory"));

   CharacterLevel=1;
   bAbilitiesInitialized=false;
}

UAbilitySystemComponent* ABaseCharacter::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void ABaseCharacter::AddStartupGameplayAbilities()
{
   check(AbilitySystemComponent);

   if (!AttributeSet)
	   GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Yellow, "No AttributeSet");

   if (HasAuthority() && !bAbilitiesInitialized)
   {
	   // Grant abilities,but only on the server
	   for (TSubclassOf<URPGGameplayAbility>& StartupAbility : GameplayAbilities)
	   {
		   AbilitySystemComponent->GiveAbility(FGameplayAbilitySpec(StartupAbility,GetCharacterLevel(),INDEX_NONE,this));
	   }

	   // Now apply passives
	   for (TSubclassOf<UGameplayEffect>& GameplayEffect : PassiveGameplayEffects)
	   {
		   FGameplayEffectContextHandle EffectContext=AbilitySystemComponent->MakeEffectContext();
		   EffectContext.AddSourceObject(this);

		   FGameplayEffectSpecHandle NewHandle=AbilitySystemComponent->MakeOutgoingSpec(GameplayEffect,GetCharacterLevel(),EffectContext);
		   if (NewHandle.IsValid())
		   {
			   FActiveGameplayEffectHandle ActiveGEHandle=AbilitySystemComponent->ApplyGameplayEffectSpecToTarget(*NewHandle.Data.Get(),AbilitySystemComponent);
		   }
	   }

	   bAbilitiesInitialized=true;
   }

}

void ABaseCharacter::RemoveStartupGameplayAbilities()
{
   check(AbilitySystemComponent);

   if (HasAuthority() && bAbilitiesInitialized)
   {
	   //Remove any abilities added from a previous call
	   TArray<FGameplayAbilitySpecHandle> AbilitiesToRemove;
	   for (const FGameplayAbilitySpec& Spec : AbilitySystemComponent->GetActivatableAbilities())
	   {
		   if ((Spec.SourceObject == nullptr) && GameplayAbilities.Contains(Spec.Ability->GetClass()))
		   {
			   AbilitiesToRemove.Add(Spec.Handle);
		   }
	   }

	   // Do in two passes so the removal happens after we have the full list
	   for (int32 i = 0; i < AbilitiesToRemove.Num(); ++i)
	   {
		   AbilitySystemComponent->ClearAbility(AbilitiesToRemove[i]);
	   }

	   // Remove all of the passive gameplay effects that were applied by this character
	   FGameplayEffectQuery Query;
	   Query.EffectSource=this;
	   AbilitySystemComponent->RemoveActiveEffects(Query);

	   bAbilitiesInitialized=false;
   }
}

bool ABaseCharacter::IsAlive()
{
   return GetHealth()>0.0f;
}

void ABaseCharacter::SetCharacterState(ECharacterState NewState)
{
	if (NewState == ECharacterState::Idle&&CharacterState!=ECharacterState::Idle)
	{
	   AddGameplayTag(CharacterIdleTag);
    }
	else if(CharacterState==ECharacterState::Idle&&NewState!=ECharacterState::Idle)
	{
	   RemoveGameplayTag(CharacterIdleTag);
	} 

	CharacterState=NewState;

}

void ABaseCharacter::SetCurrentAttackInfos(const TArray<FAttackActionInfo>& AttackInfos)
{
   CurrentAttackIndex=0;
   CurrentAttackInfos=AttackInfos;
   CurrentAttackInfo=CurrentAttackInfos[0];
}

void ABaseCharacter::ChangeToNextAttackInfo()
{
	if (CurrentAttackIndex < CurrentAttackInfos.Num() - 1)
	{
		++CurrentAttackIndex;
		CurrentAttackInfo = CurrentAttackInfos[CurrentAttackIndex];
	}
}

void ABaseCharacter::BeBlockOut()
{
   SetCharacterState(ECharacterState::BeBlockOut);
   PlayAnimMontage(BeBlockOutMontage);
}

bool ABaseCharacter::IsUsingMelee()
{
   TArray<URPGGameplayAbility*> TempArray;
   GetActiveAbilitiesWithTags(MeleeTag,TempArray);
   return TempArray.Num()>0;
}

bool ABaseCharacter::IsUsingSkill()
{
   TArray<URPGGameplayAbility*> TempArray;
   GetActiveAbilitiesWithTags(SkillTag,TempArray);
   return TempArray.Num()>0;
}

bool ABaseCharacter::CanUseAnyAbility()
{
   return IsAlive()&&!IsUsingSkill()&&CharacterState!=ECharacterState::LoseBlance;
}

void ABaseCharacter::DoMeleeAttack()
{
	
}

void ABaseCharacter::DoSkillAttack()
{
   
}

void ABaseCharacter::JumpSectionForCombo()
{
   if(!JumpSectionNS)
     return;

   if(!EnableComboPeriod)
     return;
   
   UAnimInstance* AnimInstance=GetMesh()->GetAnimInstance();
   UAnimMontage* CurrentMontage=AnimInstance->GetCurrentActiveMontage();

   AnimInstance->Montage_JumpToSection(JumpSectionNS->JumpSection,CurrentMontage);

   GEngine->AddOnScreenDebugMessage(-1,3.0,FColor::Green,(JumpSectionNS->JumpSection).ToString());

   ChangeToNextAttackInfo();

   EnableComboPeriod=false;
}

void ABaseCharacter::ChangeToLoseBlance()
{
   if(CharacterState==ECharacterState::Attack)
      StopAnimMontage(GetMesh()->GetAnimInstance()->GetCurrentActiveMontage());
   SetCharacterState(ECharacterState::LoseBlance);
   GetWorldTimerManager().SetTimer(LoseBlanceHandle,this,&ABaseCharacter::RecoverFromLoseBlance,LoseBlanceTime,false);
}

void ABaseCharacter::RecoverFromLoseBlance()
{
   SetCharacterState(ECharacterState::Idle);

   FGameplayEffectContextHandle EffectContext = AbilitySystemComponent->MakeEffectContext();
   EffectContext.AddSourceObject(this);

   FGameplayEffectSpecHandle NewHandle = AbilitySystemComponent->MakeOutgoingSpec(ResetBlanceEffect, GetCharacterLevel(), EffectContext);
   if (NewHandle.IsValid())
   {
	   FActiveGameplayEffectHandle ActiveGEHandle = AbilitySystemComponent->ApplyGameplayEffectSpecToTarget(*NewHandle.Data.Get(), AbilitySystemComponent);
   }
}

void ABaseCharacter::ToDead()
{
   GetWorldTimerManager().ClearTimer(LoseBlanceHandle);

   SetCharacterState(ECharacterState::Dead);
}

void ABaseCharacter::UpdateArm(float DeltaTime)
{
	if (!BeArmedTarget||!IsAlive()||CantArm)
		    return;

	FRotator TargetRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), BeArmedTarget->GetActorLocation());
	FRotator CurrentRotation = FMath::RInterpTo(GetControlRotation(), TargetRotation, DeltaTime, UpdataeArmSpeed);
	GetController()->SetControlRotation(FRotator(CurrentRotation.Pitch, CurrentRotation.Yaw, 0.0f));
}

void ABaseCharacter::SetBodySimulate(const FName& HitBoneName)
{
  CurrentHitBoneName=HitBoneName;

  if(CurrentHitBoneName=="pelvis")
     CurrentHitBoneName="spine_02";

  GEngine->AddOnScreenDebugMessage(-1,3.0f,FColor::Red,(CurrentHitBoneName).ToString());

  BodySimulateTimeline.PlayFromStart();
  GetWorldTimerManager().SetTimer(BodySimulateImpulseHandle,this,&ABaseCharacter::AddImpulse,ImpulseTime,false);
}

void ABaseCharacter::UpdateBodySimulate(float Rate)
{
    if(Rate<=0.0f)
	  return;
   
   GetMesh()->SetAllBodiesBelowSimulatePhysics(CurrentHitBoneName,true,true);
   GetMesh()->SetAllBodiesBelowPhysicsBlendWeight(CurrentHitBoneName,Rate);
}

void ABaseCharacter::FinishBodySimulate()
{
   GetMesh()->SetAllBodiesPhysicsBlendWeight(0.0f,false);
   GetMesh()->SetAllBodiesSimulatePhysics(false);
}

void ABaseCharacter::AddImpulse()
{
   GetMesh()->AddImpulse(CurrentHitDirection*ImpulsePower,CurrentHitBoneName,true);
}

void ABaseCharacter::OnDeathEnd()
{
     --CurrentLifeNum;
	 bCanRecover=true;
     OnDead.Broadcast(CurrentLifeNum);
}

void ABaseCharacter::ClearDamageAccumulation()
{
   CurrentDamageAccumulation=0;
}

void ABaseCharacter::Recover()
{
  GEngine->AddOnScreenDebugMessage(-1,3.0f,FColor::Red,"Try to Recover");
   
   if(CharacterState!=ECharacterState::Dead)
      return;

   if(!bCanRecover)
      return;
   
   GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Red, "Can Recover");
   GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Red, FString::SanitizeFloat(CurrentLifeNum));

   if(0>=CurrentLifeNum)
      return;

   bCanRecover=false;

   for (const FRecoverInfo& CurrentRecoverInfo : RecoverInfos)
   {
	   if (CurrentRecoverInfo.LifeNum == CurrentLifeNum)
	   {
		   PlayAnimMontage(CurrentRecoverInfo.RecoverMontage);
		   FGameplayEffectContextHandle EffectContext = AbilitySystemComponent->MakeEffectContext();
		   EffectContext.AddSourceObject(this);

		   FGameplayEffectSpecHandle NewHandle = AbilitySystemComponent->MakeOutgoingSpec(CurrentRecoverInfo.RecoverEffect, GetCharacterLevel(), EffectContext);
		   if (NewHandle.IsValid())
		   {
			   FActiveGameplayEffectHandle ActiveGEHandle = AbilitySystemComponent->ApplyGameplayEffectSpecToTarget(*NewHandle.Data.Get(), AbilitySystemComponent);
		   }

		   OnRecover.Broadcast();
	   }
   }
  
}

void ABaseCharacter::BeExecuted(UAnimMontage* Montage)
{
   if(CharacterState==ECharacterState::LoseBlance)
     GetWorldTimerManager().ClearTimer(LoseBlanceHandle);

   PlayAnimMontage(Montage);
}

void ABaseCharacter::PossessedBy(AController* NewController)
{
   Super::PossessedBy(NewController);

   // Initialize our abilities
   if (AbilitySystemComponent)
   {
	   AbilitySystemComponent->InitAbilityActorInfo(this,this);
	   AddStartupGameplayAbilities();
   }

   AddGameplayTag(CharacterIdleTag);
   AddGameplayTag(OnHealthFullTag);

   FOnTimelineFloatStatic BodySimulateTimelineUpdateCallback;
   BodySimulateTimelineUpdateCallback.BindUFunction(this,FName("UpdateBodySimulate"));
   BodySimulateTimeline.AddInterpFloat(BodySimulateCurve,BodySimulateTimelineUpdateCallback);
   FOnTimelineEventStatic BodySimulateFinishedCallback;
   BodySimulateFinishedCallback.BindUFunction(this,FName("FinishBodySimulate"));
   BodySimulateTimeline.SetTimelineFinishedFunc(BodySimulateFinishedCallback);

}

void ABaseCharacter::UnPossessed()
{
  
}

void ABaseCharacter::OnRep_Controller()
{
   Super::OnRep_Controller();

   // Our controller changed,must update ActorInfo on AbilitySystemComponent
   if (AbilitySystemComponent)
   {
	   AbilitySystemComponent->RefreshAbilityActorInfo();
   }
}

void ABaseCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
   Super::GetLifetimeReplicatedProps(OutLifetimeProps);

   DOREPLIFETIME(ABaseCharacter,CharacterLevel);
}

float ABaseCharacter::GetHealth() const
{
   if(!AttributeSet)
   return 1.f;

   return AttributeSet->GetHealth();
}

float ABaseCharacter::GetMaxHealth() const
{
  return AttributeSet->GetMaxHealth();
}

float ABaseCharacter::GetBlance() const
{
   return AttributeSet->GetBlance();

}

float ABaseCharacter::GetMaxBlance() const
{
    return AttributeSet->GetMaxBlance();
}

float ABaseCharacter::GetMoveSpeed() const
{
   return AttributeSet->GetMoveSpeed();
}

int32 ABaseCharacter::GetCharacterLevel() const
{
   return CharacterLevel;
}

bool ABaseCharacter::SetCharacterLevel(int32 NewLevel)
{
	if (CharacterLevel != NewLevel && NewLevel > 0)
	{
	    // Our level changed so we need to refresh abilities
		RemoveStartupGameplayAbilities();
		CharacterLevel=NewLevel;
		AddStartupGameplayAbilities();

		return true;
    }

	return false;
}

bool ABaseCharacter::ActivateAbilitiesWithTags(FGameplayTagContainer AbilityTags, bool bAllowRemoteActivation/*=true*/)
{
	   
	TArray<URPGGameplayAbility*> Temp;

	GetActiveAbilitiesWithTags(AbilityTags,Temp);

	if(Temp.Num()>0)
	  return false;

	if (AbilitySystemComponent)
	{
		return AbilitySystemComponent->TryActivateAbilitiesByTag(AbilityTags,bAllowRemoteActivation);
    }

	return false;
}

void ABaseCharacter::GetActiveAbilitiesWithTags(FGameplayTagContainer AbilityTags, TArray<URPGGameplayAbility*>& ActiveAbilities)
{
	if (AbilitySystemComponent)
	{
	   AbilitySystemComponent->GetActiveAbilitiesWithTags(AbilityTags,ActiveAbilities);
    }
}

bool ABaseCharacter::GetCooldownRemainingForTag(FGameplayTagContainer CooldownTags, float& TimeRemaining, float& CooldownDuration)
{
	if (AbilitySystemComponent && CooldownTags.Num() > 0)
	{
		TimeRemaining=0.f;
		CooldownDuration=0.f;

	    FGameplayEffectQuery const Query=FGameplayEffectQuery::MakeQuery_MatchAnyEffectTags(CooldownTags);
		TArray<TPair<float,float>> DurationAndTimeRemaining=AbilitySystemComponent->GetActiveEffectsTimeRemainingAndDuration(Query);
		if (DurationAndTimeRemaining.Num() > 0)
		{
			int32 BestIdx=0;
			float LongestTime=DurationAndTimeRemaining[0].Key;
			for (int32 Idx = 1; Idx < DurationAndTimeRemaining.Num(); ++Idx)
			{
				if (DurationAndTimeRemaining[Idx].Key > LongestTime)
				{
					LongestTime=DurationAndTimeRemaining[Idx].Key;
					BestIdx=Idx;
				}
			}

			TimeRemaining=DurationAndTimeRemaining[BestIdx].Key;
			CooldownDuration=DurationAndTimeRemaining[BestIdx].Value;

			return true;
		}
	}

	return false;
}

void ABaseCharacter::AddGameplayTag(const FGameplayTag& TagToAdd)
{
    if(!TagToAdd.IsValid())
	   return;
   GetAbilitySystemComponent()->AddLooseGameplayTag(TagToAdd);
   GetAbilitySystemComponent()->SetTagMapCount(TagToAdd,1);
}

void ABaseCharacter::RemoveGameplayTag(FGameplayTag TagToRemove)
{
  if(!TagToRemove.IsValid())
     return;
  GetAbilitySystemComponent()->RemoveLooseGameplayTag(TagToRemove);
}

void ABaseCharacter::OnDamaged(float DamageAmount, const FHitResult& HitInfo, const struct FGameplayTagContainer& DamageTags, ABaseCharacter* InstigatorCharacter, AActor* DamageCauser)
{
    GetWorldTimerManager().ClearTimer(ClearDamageAccumulationHandle);

	if(CharacterState==ECharacterState::Dead)
	   return;

	CurrentDamageAccumulation+=DamageAmount;

	GetWorldTimerManager().SetTimer(ClearDamageAccumulationHandle, this, &ABaseCharacter::ClearDamageAccumulation, ClearDamageAccumulationWaitTime, false);
}

void ABaseCharacter::OnHealthChanged(float DeltaValue, const struct FGameplayTagContainer& EventTags)
{
    if(CharacterHUD)
	   CharacterHUD->UpdateHealthBar(DeltaValue,GetHealth(),GetMaxHealth());

	if (CharacterState!=ECharacterState::Dead&&!IsAlive())
	{
		ToDead();
	}
	  
}

void ABaseCharacter::OnBlanceChanged(float DeltaValue, const struct FGameplayTagContainer& EventTags)
{
    if(GetBlance()==GetMaxBlance()&&CharacterState!=ECharacterState::LoseBlance&&CharacterState!=ECharacterState::Dead)
	     ChangeToLoseBlance();

	if (GetBlance() == 0)
		RemoveGameplayTag(OnBlanceNotZeroTag);
	else if (GetBlance() - DeltaValue == 0)
		AddGameplayTag(OnBlanceNotZeroTag);
   
   if(CharacterHUD)
      CharacterHUD->UpdateBlanceBar(GetBlance(),GetMaxBlance());

}

void ABaseCharacter::OnMoveSpeedChanged(float DeltaValue, const struct FGameplayTagContainer& EventTags)
{

}

void ABaseCharacter::HandleDamage(float DamgeAmount, const FHitResult& HitInfo, const struct FGameplayTagContainer& DamageTags, ABaseCharacter* InstigatorCharacter, AActor* DamageCauser)
{
   OnDamaged(DamgeAmount,HitInfo,DamageTags,InstigatorCharacter,DamageCauser);
}

void ABaseCharacter::HandleHealthChanged(float DeltaValue, const struct FGameplayTagContainer& EventTags)
{
   // We only call the BP callback if this is not the initial ability setup
	if (bAbilitiesInitialized)
	{
       OnHealthChanged(DeltaValue,EventTags);
    }
}

void ABaseCharacter::HandleBlanceChanged(float DeltaValue, const struct FGameplayTagContainer& EventTags)
{
	if (bAbilitiesInitialized)
	{
	   OnBlanceChanged(DeltaValue,EventTags);
    }
}

void ABaseCharacter::HandleMoveSpeedChanged(float DeltaValue, const struct FGameplayTagContainer& EventTags)
{
    // Update the character movement's walk speed
 	//GetCharacterMovement()->MaxWalkSpeed=GetMoveSpeed();
 
 	if (bAbilitiesInitialized)
 	{
 		OnMoveSpeedChanged(DeltaValue,EventTags);
 	}

}

void ABaseCharacter::Tick(float DeltaTime)
{
   Super::Tick(DeltaTime);

   UpdateArm(DeltaTime);
   BodySimulateTimeline.TickTimeline(DeltaTime);
}

void ABaseCharacter::SetJumpSection(UJumpSectionNS* _JumpSectionNS, bool _EnableCombo)
{
   JumpSectionNS=_JumpSectionNS;
   EnableComboPeriod=_EnableCombo;
}

