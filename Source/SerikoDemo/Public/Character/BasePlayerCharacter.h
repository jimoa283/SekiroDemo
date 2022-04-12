// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/BaseCharacter.h"
#include "BasePlayerCharacter.generated.h"


class AWeaponActor;
class USoundCue;


/**
 * 
 */
UCLASS()
class SERIKODEMO_API ABasePlayerCharacter : public ABaseCharacter
{
	GENERATED_BODY()

public:
    
	ABasePlayerCharacter();

protected:

	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category=Component)
	UStaticMeshComponent* Scabbard;

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category=Weapon)
	FName KatanaSlot;

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category=Weapon)
	TSubclassOf<AWeaponActor> WeaponClass;

	UPROPERTY(EditAnywhere,Category=Move)
	float RunSpeed;

	UPROPERTY(EditAnywhere,Category=Move)
	float WalkSpeed;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Move)
	FVector2D MoveAxis;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
	float DeltaTimes;

	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category=Defense)
	float BlockTime;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Defense)
	float StartBlockTime;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category=Defense)
	bool bIsTurningToDefense;

	FTimerHandle StartBlockHandle;

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category=Abilities)
	FGameplayTagContainer DefenseTag;

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category=Abilities)
	FGameplayTag BlockHitTag;
	
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category=Abilities)
	FGameplayTag DefendHitTag;

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category=Abilities)
	FGameplayTag EndDefenseTag;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Abilities)
	FGameplayTagContainer AvoidTag;

	FTimerHandle DefenseHandle;

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category=Hit)
	UParticleSystem* DamageBloodPS;

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category=Hit)
	USoundCue* WeaponSlideSC;

	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite,Category=Hit)
	bool IsLeft;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category=Equip)
	bool bIsEquip;

	//UFUNCTION()
	virtual void DoMeleeAttack() override;

	UFUNCTION(BlueprintCallable)
	virtual void DoDefense();

	UFUNCTION(BlueprintCallable)
	virtual void StartBlock();

	UFUNCTION()
	virtual void ChangeToDefense();

	UFUNCTION(BlueprintCallable)
	virtual void CancelDefense();

	UFUNCTION(BlueprintCallable)
	void EndDefense();

	UFUNCTION(BlueprintCallable)
	void SetWalkState();

	UFUNCTION(BlueprintCallable)
	void SetRunState();

	virtual void OnDamaged(float DamageAmount, const FHitResult& HitInfo, const struct FGameplayTagContainer& DamageTags, ABaseCharacter* InstigatorCharacter, AActor* DamageCauser) override;

	virtual void OnBlanceChanged(float DeltaValue,const struct FGameplayTagContainer& EventTags) override;
	
	FName GetHitDirection(FVector AttackDirection);

	UFUNCTION(BlueprintImplementableEvent)
	void PlayHitMontage(FName HitDirection);

	UFUNCTION(BlueprintCallable)
	void MoveForward(float AxisValue);

	UFUNCTION(BlueprintCallable)
	void MoveRight(float AxisValue); 

	UFUNCTION(BlueprintCallable)
	void DoAvoid();

public:

	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	void BlockOrDefenseTarget(ABaseCharacter* Target,const FHitResult& OutHit,bool bBlock);

	virtual void SetCharacterState(ECharacterState NewState) override;

};
