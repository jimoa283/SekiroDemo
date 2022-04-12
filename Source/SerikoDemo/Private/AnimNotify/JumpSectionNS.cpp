// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotify/JumpSectionNS.h"
#include "Character/BaseCharacter.h"

void UJumpSectionNS::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration);
    
	if (ABaseCharacter* Character = MeshComp->GetOwner<ABaseCharacter>())
	{
	   Character->SetJumpSection(this,true);
	}
}

void UJumpSectionNS::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::NotifyEnd(MeshComp, Animation);
	
	if (ABaseCharacter* Character = MeshComp->GetOwner<ABaseCharacter>())
	{
		Character->SetJumpSection(nullptr,false);
	}
}
