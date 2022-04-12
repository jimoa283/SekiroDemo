// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotify/WeaponAttackNS.h"
#include "Character/BaseCharacter.h"
#include "Weapon/WeaponActor.h"

void UWeaponAttackNS::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration);
	
	if (ABaseCharacter* Character = MeshComp->GetOwner<ABaseCharacter>())
	{
		AWeaponActor* Weapon=Character->GetCurrentWeapon();
		if(!Weapon)
		  return;

		Weapon->EnableTrace(EventTag,OpenTraceCount);
	}
}

void UWeaponAttackNS::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::NotifyEnd(MeshComp, Animation);
	
	if (ABaseCharacter* Character = MeshComp->GetOwner<ABaseCharacter>())
	{
		AWeaponActor* Weapon = Character->GetCurrentWeapon();
		if (!Weapon)
			return;

		Weapon->DisableTrace();
	}
}
