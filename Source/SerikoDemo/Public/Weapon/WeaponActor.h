// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Item/RPGTypes.h"
#include "Character/AttackActionInfo.h"
#include "Ability/RPGGameplayAbility.h"
#include "WeaponActor.generated.h"

class USoundCue;
class URPGWeaponItem;
class ABasePlayerCharacter;

UCLASS()
class SERIKODEMO_API AWeaponActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWeaponActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category=Component)
	USceneComponent* DefaultSceneRoot;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category=Component)
	UStaticMeshComponent* StaticMesh;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category=Weapon)
	ABasePlayerCharacter* Character;

	FName CharacterTag;

	FGameplayTag AttackEventTag;

	UPROPERTY(EditDefaultsOnly,Category=Weapon)
	FGameplayTag DefenseEventTag;

	UPROPERTY(EditDefaultsOnly,Category=Weapon)
	FGameplayTag BlockEventTag;

	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category=Weapon)
	int32 TracePointCount;

	float TracePointDistance;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category=Weapon)
	TArray<FName> SocketNames;

	TMap<FName,FVector> PreSocketLocation;

	TArray<AActor*> DamagedActor;

	bool TraceOpen;

	FVector Start;

	FVector End;

	FVector HitLocation;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category=Weapon)
	int32 OpenTracePointCount;

	FAttackActionInfo CurrentAttackActionInfo;

	void Trace();

	void UpdateSockets();

	UFUNCTION(BlueprintImplementableEvent)
	void HitPause();

	void GetTracePointDistance();

	UFUNCTION(BlueprintImplementableEvent)
	void AddTracePoint();

public:	

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ExposeOnSpawn = "true"), Category = WeaponItem)
	URPGWeaponItem* RPGWeaponItem;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ExposeOnSpawn = "true"),Category = WeaponItem)
	FRPGItemSlot Slot;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void InitWeaponSockets(ABasePlayerCharacter* _Character,FName _CharacterTag);

	void EnableTrace(const FGameplayTag& EventTag, int32 OpenCount);

	void DisableTrace();
};
