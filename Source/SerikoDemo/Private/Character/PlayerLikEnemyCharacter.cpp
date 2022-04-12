// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/PlayerLikEnemyCharacter.h"
#include "Components/WidgetComponent.h"
#include "UI/CharacterHUD.h"
#include "Item/MoneyEnemyDrop.h"
#include "Kismet/GameplayStatics.h"
#include "Character/PlayerCharacter.h"
#include "Components/CapsuleComponent.h"

APlayerLikEnemyCharacter::APlayerLikEnemyCharacter()
{
   BeArmedWidget=CreateDefaultSubobject<UWidgetComponent>(TEXT("BeArmedWidget"));
   BeArmedWidget->SetupAttachment(GetMesh(),TEXT("spine_03"));

   LoseArmedWidget=CreateDefaultSubobject<UWidgetComponent>(TEXT("LoseArmedWidget"));
   LoseArmedWidget->SetupAttachment(GetMesh(),TEXT("spine_03"));

   HUDWidget=CreateDefaultSubobject<UWidgetComponent>(TEXT("HUDWidget"));
   HUDWidget->SetupAttachment(RootComponent);
}

bool APlayerLikEnemyCharacter::IsShowing()
{
	return !LoseArmedWidget->bHiddenInGame;
}

void APlayerLikEnemyCharacter::ShowCanBeExecute()
{
	LoseArmedWidget->SetHiddenInGame(false);
}

void APlayerLikEnemyCharacter::HideCanbeExecute()
{
	LoseArmedWidget->SetHiddenInGame(true);
}

void APlayerLikEnemyCharacter::SetBeArmed()
{
	BeArmedWidget->SetHiddenInGame(false);
}

void APlayerLikEnemyCharacter::CancelBeArmed()
{
	BeArmedWidget->SetHiddenInGame(true);
}

bool APlayerLikEnemyCharacter::CanShow()
{
	return IsAlive()&&BeArmedTarget==nullptr;
}

bool APlayerLikEnemyCharacter::CanHide()
{
	return CharacterState!=ECharacterState::LoseBlance;
}

void APlayerLikEnemyCharacter::BeginPlay()
{
   Super::BeginPlay();

   BeArmedWidget->SetHiddenInGame(true);
   LoseArmedWidget->SetHiddenInGame(true);

   if(HUDWidget->GetWidget())
     CharacterHUD=Cast<UCharacterHUD>(HUDWidget->GetWidget());

   bIsEquip=bInitEquip;
}

void APlayerLikEnemyCharacter::OnDeathEnd()
{
	Super::OnDeathEnd();

	HUDWidget->SetHiddenInGame(true);

	/*GetMesh()->SetCollisionProfileName(TEXT("Ragdoll"));
    GetMesh()->SetSimulatePhysics(true);

	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);*/

	StartRagDoll();

	Cast<APlayerCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0))->RemoveShowBackEnemy(this);

	FTransform SpawnTransform(GetMesh()->GetComponentLocation());
	AMoneyEnemyDrop* Drop=GetWorld()->SpawnActorDeferred<AMoneyEnemyDrop>(MoneyDropClass,SpawnTransform);

	if (Drop)
	{
		Drop->Init(Money);
	    UGameplayStatics::FinishSpawningActor(Drop,SpawnTransform);
	}

}


void APlayerLikEnemyCharacter::ChangeToLoseBlance()
{
	Super::ChangeToLoseBlance();

	ShowCanBeExecute();

	Cast<APlayerCharacter>(UGameplayStatics::GetPlayerCharacter(this,0))->TargetLoseBlanceHitPause(this);
}

void APlayerLikEnemyCharacter::RecoverFromLoseBlance()
{
	Super::RecoverFromLoseBlance();

	HideCanbeExecute();
}


