#pragma once
#include "CoreMinimal.h"
#include "AttackActionInfo.generated.h"

UENUM(BlueprintType)
enum class EAttackType:uint8
{
   Sword
};

UENUM(BlueprintType)
enum class EDangerType :uint8
{
    None,
    Spike,
    DownAttack
};

USTRUCT(BlueprintType)
struct FAttackActionInfo
{
	GENERATED_BODY()

public:

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = AttackActionInfo)
    float BaseHealthDamage;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = AttackActionInfo)
    float BaseBlanceDamage;

    UPROPERTY(EditAnywhere,BlueprintReadOnly,Category=AttackActionInfo)
    bool bCanHitMontage;
	
    UPROPERTY(EditAnywhere,BlueprintReadOnly,Category=AttackActionInfo)
    EAttackType AttackType;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = AttackActionInfo)
    EDangerType DangerType;

    UPROPERTY(EditAnywhere,BlueprintReadOnly,Category=AttackActionInfo)
    bool bCantBeBlockOut;

    UPROPERTY(EditAnywhere,BlueprintReadOnly,Category=AttackActionInfo)
    bool bIsLongAttack;

};

