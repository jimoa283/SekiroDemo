// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "Net/UnrealNetwork.h"
#include "Ability/RPGAttributeSet.h"
#include "Ability/RPGAbilitySystemComponent.h"
#include "GameFramework/Character.h"
#include "UObject/ScriptInterface.h"
#include "Character/AttackActionInfo.h"
#include "Character/RPGInventoryInterface.h"
#include "Components/TimelineComponent.h"
#include "BaseCharacter.generated.h"

class URPGGameplayAbility;
class UGameplayEffect;
class UJumpSectionNS;
class AWeaponActor;
class UInventoryComponent;
class UCharacterHUD;
class UCurveFloat;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDead,int32,LifeNum);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnRecover);

UENUM(BlueprintType)
enum class ECharacterState :uint8
{
	Idle,
	Crouch,
	Jump,
	Attack,
	Block,
	BeBlockOut,
	Defend,
	Hit,
	Avoid,
	LoseBlance,
	Dead,
	Grapple,
	WallHug,
	HangLedge
};

USTRUCT(BlueprintType)
struct FRecoverInfo
{
	GENERATED_BODY()

public: 
    
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category=Recover)
	int32 LifeNum;

	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category=Recover)
	UAnimMontage* RecoverMontage;

	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category=Recover)
	TSubclassOf<UGameplayEffect> RecoverEffect;
};

UCLASS(Abstract)
class SERIKODEMO_API ABaseCharacter : public ACharacter,public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ABaseCharacter();
	virtual void PossessedBy(AController* NewController) override;
	virtual void UnPossessed() override;
	virtual void OnRep_Controller() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	UFUNCTION(BlueprintCallable)
	virtual float GetHealth() const;
	
	UFUNCTION(BlueprintCallable)
	virtual float GetMaxHealth() const;

	UFUNCTION(BlueprintCallable)
	virtual float GetBlance() const;

	UFUNCTION(BlueprintCallable)
	virtual float GetMaxBlance() const;

	UFUNCTION(BlueprintCallable)
    virtual float GetMoveSpeed() const;

	UFUNCTION(BlueprintCallable)
	virtual int32 GetCharacterLevel() const;

	UFUNCTION(BlueprintCallable)
	virtual bool SetCharacterLevel(int32 NewLevel);

	UFUNCTION(BlueprintCallable,Category="Abilities")
	bool ActivateAbilitiesWithTags(FGameplayTagContainer AbilityTags,bool bAllowRemoteActivation=true);

	/* Returns a list of active abilities matching the specified tags. This only returns if the ability is currently running */
	UFUNCTION(BlueprintCallable,Category="Abilities")
	void GetActiveAbilitiesWithTags(FGameplayTagContainer AbilityTags,TArray<URPGGameplayAbility*>& ActiveAbilities);
	 
	/* Returns total time and remaining time for cooldown tags. Returns false if no active cooldowns found */
	UFUNCTION(BlueprintCallable,Category="Abilities")
	bool GetCooldownRemainingForTag(FGameplayTagContainer CooldownTags,float& TimeRemaining,float& CooldownDuration);

	UFUNCTION(BlueprintCallable,Category="Abilities")
	void AddGameplayTag(const FGameplayTag& TagToAdd);

	UFUNCTION(BlueprintCallable,Category="Abilities")
	void RemoveGameplayTag(FGameplayTag TagToRemove);

protected:

    UPROPERTY(EditAnywhere,BlueprintReadOnly,Category=Base)
	FName CharacterTag;

    UPROPERTY(EditAnywhere,BlueprintReadOnly,Category=Base)
	ECharacterState CharacterState;

	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category=Base)
	int32 MaxLifeNum;

	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category=Base)
	int32 CurrentLifeNum;

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category=Abilities)
	FGameplayTagContainer MeleeTag;

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category=Abilities)
	FGameplayTagContainer SkillTag;

	UPROPERTY(EditAnywhere,Replicated,Category=Abilities)
	int32 CharacterLevel;
	
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category=Abilities)
	TArray<TSubclassOf<URPGGameplayAbility>> GameplayAbilities;

	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category=Abilities)
	TMap<FRPGItemSlot,TSubclassOf<URPGGameplayAbility>> DefaultSlottedAbilities;

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category=Abilities)
	TArray<TSubclassOf<UGameplayEffect>> PassiveGameplayEffects;

	UPROPERTY()
	URPGAbilitySystemComponent* AbilitySystemComponent;

	UPROPERTY(VisibleAnywhere,Category=Abilities)
	URPGAttributeSet* AttributeSet;

	UPROPERTY()
	int32 bAbilitiesInitialized;

	UPROPERTY()
	UJumpSectionNS* JumpSectionNS;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Arm)
	ABaseCharacter* BeArmedTarget;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Arm)
	float UpdataeArmSpeed;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category=Arm)
	bool CantArm;

	UPROPERTY()
	bool EnableComboPeriod;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Weapon)
	AWeaponActor* CurrentWeapon;

	UPROPERTY(VisibleInstanceOnly,BlueprintReadOnly,Category=HUD)
	UCharacterHUD* CharacterHUD;

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category=Abilities)
	FGameplayTag OnHealthFullTag;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Abilities)
    FGameplayTag OnBlanceNotZeroTag;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Abilities)
	FGameplayTag CharacterIdleTag;

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category=Abilities)
	TSubclassOf<UGameplayEffect> ResetBlanceEffect;

	UPROPERTY(BlueprintReadOnly,Category=AttackActionInfo)
	FAttackActionInfo CurrentAttackInfo;

	UPROPERTY(BlueprintReadOnly,Category=AttackActionInfo)
	TArray<FAttackActionInfo> CurrentAttackInfos;

	int32 CurrentAttackIndex;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category=Base)
	bool bIsSuperArmor;

	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category=Base)
	float LoseBlanceTime;

	FTimerHandle LoseBlanceHandle;

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category=BeBlockOut)
	UAnimMontage* BeBlockOutMontage;

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category=BodySimulate)
	UCurveFloat* BodySimulateCurve;

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category=BodySimulate)
	float ImpulseTime;

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category=BodySimulate)
	float ImpulsePower;

	FTimeline BodySimulateTimeline;

	FName CurrentHitBoneName;
	 
	FTimerHandle BodySimulateImpulseHandle;

	FVector CurrentHitDirection;

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category=DamageAccumulationCheck)
	float CurrentDamageAccumulation;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = DamageAccumulationCheck)
	float ClearDamageAccumulationWaitTime;

	FTimerHandle ClearDamageAccumulationHandle;

	UPROPERTY(BlueprintAssignable,Category=Dead)
	FOnDead OnDead;

	UPROPERTY(EditDefaultsOnly,Category=Dead)
	bool bCanRecover;

	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category=Dead)
	TArray<FRecoverInfo> RecoverInfos;

	UPROPERTY(BlueprintAssignable,Category=Dead)
	FOnRecover OnRecover;

	UFUNCTION()
	virtual void OnDamaged(float DamageAmount,const FHitResult& HitInfo,const struct FGameplayTagContainer& DamageTags,ABaseCharacter* InstigatorCharacter,AActor* DamageCauser);

	UFUNCTION()
	virtual void OnHealthChanged(float DeltaValue,const struct FGameplayTagContainer& EventTags);

	UFUNCTION()
	virtual void OnBlanceChanged(float DeltaValue,const struct FGameplayTagContainer& EventTags);

	UFUNCTION()
	virtual void OnMoveSpeedChanged(float DeltaValue,const struct FGameplayTagContainer& EventTags);

	/* Apply the startup gameplay abilities and effects */
	virtual void AddStartupGameplayAbilities();

	/* Attempts to remove any startup gameplay abilities */
	virtual void RemoveStartupGameplayAbilities();

	UFUNCTION(BlueprintPure,BlueprintCallable)
	bool IsUsingMelee();

	UFUNCTION(BlueprintPure,BlueprintCallable)
	bool IsUsingSkill();

	UFUNCTION(BlueprintPure,BlueprintCallable)
	bool CanUseAnyAbility();

	UFUNCTION(BlueprintCallable)
	virtual void DoMeleeAttack();
	 
	UFUNCTION(BlueprintCallable)
	virtual void DoSkillAttack();

	UFUNCTION()
    virtual void JumpSectionForCombo();

	UFUNCTION(BlueprintCallable)
	virtual void ChangeToLoseBlance();

	UFUNCTION()
	virtual void RecoverFromLoseBlance();

	UFUNCTION()
	virtual void ToDead();

	UFUNCTION()
	virtual void UpdateArm(float DeltaTime);

	UFUNCTION()
	virtual void SetBodySimulate(const FName& HitBoneName);

	UFUNCTION()
	void UpdateBodySimulate(float Rate);

	UFUNCTION()
	void FinishBodySimulate();

	UFUNCTION()
	void AddImpulse();

	UFUNCTION(BlueprintCallable)
	virtual void OnDeathEnd();

	UFUNCTION(BlueprintCallable)
	void ClearDamageAccumulation();

	UFUNCTION(BlueprintCallable)
	virtual void Recover();

	virtual void HandleDamage(float DamgeAmount,const FHitResult& HitInfo,const struct FGameplayTagContainer& DamageTags,ABaseCharacter* InstigatorCharacter,AActor* DamageCauser);
	virtual void HandleHealthChanged(float DeltaValue,const struct FGameplayTagContainer& EventTags);
	virtual void HandleBlanceChanged(float DeltaValue,const struct FGameplayTagContainer& EventTags);
	virtual void HandleMoveSpeedChanged(float DeltaValue,const struct FGameplayTagContainer& EventTags);

    friend URPGAttributeSet;

public: 

	virtual void Tick(float DeltaTime) override;
    
	void SetJumpSection(UJumpSectionNS* _JumpSectionNS,bool _EnableCombo);

	UFUNCTION(BlueprintPure)
	bool IsAlive();

	FORCEINLINE AWeaponActor* GetCurrentWeapon(){return CurrentWeapon;}

	FORCEINLINE ECharacterState GetCharacterState() {return CharacterState; }

	UFUNCTION(BlueprintCallable)
	virtual void SetCharacterState(ECharacterState NewState);

	FORCEINLINE FAttackActionInfo GetCurrentAttackInfo() const {return CurrentAttackInfo;}

	UFUNCTION(BlueprintCallable)
	void SetCurrentAttackInfos(const TArray<FAttackActionInfo>& AttackInfos);

	UFUNCTION(BlueprintCallable)
	void ChangeToNextAttackInfo();

	UFUNCTION(BlueprintCallable)
	virtual void BeBlockOut();

	UFUNCTION()
	virtual void BeExecuted(UAnimMontage* Montage);

	UFUNCTION(BlueprintCallable)
	FORCEINLINE ABaseCharacter* GetArmTarget(){return BeArmedTarget;}
};
