// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/BasePlayerCharacter.h"
#include "Character/ShowExecuteInterface.h"
#include "Character/BeArmedInterface.h"
#include "PlayerLikEnemyCharacter.generated.h"

class UWidgetComponent;
class AMoneyEnemyDrop;

/**
 * 
 */
UCLASS()
class SERIKODEMO_API APlayerLikEnemyCharacter : public ABasePlayerCharacter,public IBeArmedInterface
{
	GENERATED_BODY()

public:
     
	APlayerLikEnemyCharacter();


	virtual bool IsShowing() override;

protected:

    virtual void BeginPlay() override;

    UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category=Component)
	UWidgetComponent* BeArmedWidget;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category=Component)
	UWidgetComponent* HUDWidget;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category=Component)
	UWidgetComponent* LoseArmedWidget;

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category=Abilities)
	TArray<FGameplayTagContainer> NormalAttacks;

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category=Abilities)
	TArray<FGameplayTagContainer> DangerAttacks;

	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category=Money)
	int32 Money;

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category=Money)
	TSubclassOf<AMoneyEnemyDrop> MoneyDropClass;

	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category=Equip)
	bool bInitEquip;

	virtual void OnDeathEnd() override;

	UFUNCTION(BlueprintImplementableEvent,BlueprintCallable)
	void StartRagDoll();

public:

	virtual void ChangeToLoseBlance() override;

	virtual void RecoverFromLoseBlance() override;
	
	virtual bool CanShow() override;

	virtual bool CanHide() override;

	virtual void ShowCanBeExecute() override;

	virtual void HideCanbeExecute() override;

	virtual void SetBeArmed() override;

	virtual void CancelBeArmed() override;
};
