// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/WeaponActor.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Item/RPGWeaponItem.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Character/BasePlayerCharacter.h"
#include "Ability/RPGAbilitySystemComponent.h"

// Sets default values
AWeaponActor::AWeaponActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	StaticMesh=CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	//StaticMesh->SetupAttachment(DefaultSceneRoot);
	RootComponent=StaticMesh;
}

// Called when the game starts or when spawned
void AWeaponActor::BeginPlay()
{
	Super::BeginPlay();
	GetTracePointDistance();

}

void AWeaponActor::Trace()
{
	//for (const FName& SocketName : SocketNames)
	//{
		for (int32 i = 0; i < OpenTracePointCount; ++i)
		{
			Start = *PreSocketLocation.Find(SocketNames[i]);
			End = StaticMesh->GetSocketLocation(SocketNames[i]);

			TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
			ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_Pawn));
			ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_WorldDynamic));

			TArray<FHitResult> OutHits;

			UKismetSystemLibrary::LineTraceMultiForObjects(this, Start, End, ObjectTypes, false, DamagedActor, EDrawDebugTrace::None, OutHits, true);

			for (const FHitResult& OutHit : OutHits)
			{
				HitLocation = OutHit.Location;
				AActor* Target = OutHit.GetActor();
				if (!Target->Tags.Contains(CharacterTag))
				{
					if (DamagedActor.Contains(Target))
					{
						continue;
					}
					else
					{
						DamagedActor.AddUnique(Target);
						if (Target->ActorHasTag("Character"))
						{
							ABaseCharacter* TargetCharacter = Cast<ABaseCharacter>(Target);
							ECharacterState TargetCharacterState = TargetCharacter->GetCharacterState();

							if(!TargetCharacter->IsAlive())
							     return;

							FVector AttackDirection = FVector((Start - End).X, (Start - End).Y, 0.0f);
							float DotValue = FVector::DotProduct(AttackDirection, Target->GetActorForwardVector());

							if (DotValue > 0 && TargetCharacterState == ECharacterState::Block)
							{
								GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Cyan, "Target Is Block");

								if (!CurrentAttackActionInfo.bCantBeBlockOut && !CurrentAttackActionInfo.bIsLongAttack)
									Character->BeBlockOut();

								ABasePlayerCharacter* PlayerLikeCharacter = Cast<ABasePlayerCharacter>(TargetCharacter);
								PlayerLikeCharacter->BlockOrDefenseTarget(Character, OutHit, true);

								HitPause();
							}
							else if (DotValue > 0 && TargetCharacterState == ECharacterState::Defend && CurrentAttackActionInfo.DangerType == EDangerType::None)
							{
								GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Cyan, "Target Is Defend");
								ABasePlayerCharacter* PlayerLikeCharacter = Cast<ABasePlayerCharacter>(TargetCharacter);
								PlayerLikeCharacter->BlockOrDefenseTarget(Character, OutHit, false);
							}
							else
							{
								FGameplayEventData GameplayEventData;
								GameplayEventData.Instigator = Character;
								GameplayEventData.Target = Target;
								FGameplayEffectContextHandle EffectContext = Character->GetAbilitySystemComponent()->MakeEffectContext();
								EffectContext.AddHitResult(OutHit);
								GameplayEventData.ContextHandle = EffectContext;
								UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(Character, AttackEventTag, GameplayEventData);
							}
						}
					}
				}
			}
	    }
	   
	//}
}

void AWeaponActor::UpdateSockets()
{
	//for (const FName& SocketName : SocketNames)
	//{
	//   PreSocketLocation[SocketName]=StaticMesh->GetSocketLocation(SocketName);
 //   }

	for (int32 i = 0; i < OpenTracePointCount; i++)
	{
		PreSocketLocation[SocketNames[i]] = StaticMesh->GetSocketLocation(SocketNames[i]);
	}
}

void AWeaponActor::GetTracePointDistance()
{
   float Distance=(StaticMesh->GetSocketLocation("TraceEnd") - StaticMesh->GetSocketLocation("TraceStart")).Size();
   TracePointDistance=Distance/TracePointCount;
}

// Called every frame
void AWeaponActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (TraceOpen)
	{
		Trace();
		UpdateSockets();
	}
}

void AWeaponActor::InitWeaponSockets(ABasePlayerCharacter* _Character, FName _CharacterTag)
{
   Character=_Character;
   SocketNames=StaticMesh->GetAllSocketNames();
   CharacterTag=_CharacterTag;
   PreSocketLocation.Empty();
   TraceOpen=false;
   for (const FName& SocketName : SocketNames)
   {
	   PreSocketLocation.Add(SocketName, StaticMesh->GetSocketLocation(SocketName));
   }
}

void AWeaponActor::EnableTrace(const FGameplayTag& EventTag, int32 OpenCount)
{
   if(TraceOpen)
     return;

   OpenTracePointCount=FMath::Clamp(OpenCount,0,SocketNames.Num());
   UpdateSockets();

   TraceOpen = true;

   AttackEventTag=EventTag;
   CurrentAttackActionInfo=Character->GetCurrentAttackInfo();
}

void AWeaponActor::DisableTrace()
{
   if(!TraceOpen)
     return;

   TraceOpen=false;
   DamagedActor.Empty();
}

