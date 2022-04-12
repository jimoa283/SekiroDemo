// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/BasePlayerCharacter.h"
#include "Weapon/WeaponActor.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework\CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Sound\SoundCue.h"
#include "AbilitySystemBlueprintLibrary.h"

ABasePlayerCharacter::ABasePlayerCharacter()
{
    Scabbard=CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Scabbard"));
    Scabbard->SetupAttachment(GetMesh(),TEXT("Scabbard_Target01"));
}

void ABasePlayerCharacter::BeginPlay()
{
    Super::BeginPlay();

    if (WeaponClass)
    {
        FTransform SpawnTransform;
        CurrentWeapon= GetWorld()->SpawnActorDeferred<AWeaponActor>(WeaponClass, SpawnTransform);
        if (CurrentWeapon)
        {
            CurrentWeapon->InitWeaponSockets(this,CharacterTag);
            UGameplayStatics::FinishSpawningActor(CurrentWeapon,SpawnTransform);
        }

        if(CurrentWeapon)
        CurrentWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, KatanaSlot);
    }
    
}

void ABasePlayerCharacter::MoveForward(float AxisValue)
{
    MoveAxis.Y = AxisValue;

    if ((Controller != nullptr) && (AxisValue != 0.0f))
    {
        // find out which way is forward
        const FRotator Rotation = Controller->GetControlRotation();
        const FRotator YawRotation(0, Rotation.Yaw, 0);

        // get forward vector
        const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

        GEngine->AddOnScreenDebugMessage(-1,0.0f,FColor::Cyan,"Forward");

        AddMovementInput(Direction, AxisValue);
    }
}

void ABasePlayerCharacter::MoveRight(float AxisValue)
{
    MoveAxis.X = AxisValue;

    //if (CharacterState != ECharacterState::Idle && CharacterState != ECharacterState::Jump)
    //    return;

    if ((Controller != nullptr) && (AxisValue != 0.0f))
    {
        // find out which way is right
        const FRotator Rotation = Controller->GetControlRotation();
        const FRotator YawRotation(0, Rotation.Yaw, 0);

        // get right vector 
        const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

        GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Cyan, "Right");

        // add movement in that direction
        AddMovementInput(Direction, AxisValue);
    }
}

void ABasePlayerCharacter::DoAvoid()
{
    if (CharacterState != ECharacterState::Idle)
        return;

    SetCharacterState(ECharacterState::Avoid);
    ActivateAbilitiesWithTags(AvoidTag);
}

void ABasePlayerCharacter::DoMeleeAttack()
{
    if (CharacterState==ECharacterState::Attack)
    {
       JumpSectionForCombo();
    }
    else
    {
       ActivateAbilitiesWithTags(MeleeTag);
    }
}

void ABasePlayerCharacter::DoDefense()
{
    if (!CanUseAnyAbility())
        return;

    if (CharacterState != ECharacterState::Idle)
        return;

    bIsTurningToDefense=true;

    SetWalkState();
    GetWorld()->GetTimerManager().SetTimer(StartBlockHandle,this,&ABasePlayerCharacter::StartBlock,StartBlockTime,false);
}

void ABasePlayerCharacter::StartBlock()
{
    if (!CanUseAnyAbility())
        return;

    if (CharacterState != ECharacterState::Idle)
           return;

    //StopAnimMontage(GetMesh()->GetAnimInstance()->GetCurrentActiveMontage());

    SetCharacterState(ECharacterState::Block);
    SetWalkState();
    GetWorld()->GetTimerManager().SetTimer(DefenseHandle, this, &ABasePlayerCharacter::ChangeToDefense, BlockTime, false);
    ActivateAbilitiesWithTags(DefenseTag);
}

void ABasePlayerCharacter::ChangeToDefense()
{
   SetCharacterState(ECharacterState::Defend);
}

void ABasePlayerCharacter::CancelDefense()
{ 
   if(CharacterState!=ECharacterState::Defend&&CharacterState!=ECharacterState::Block)
        return;
   
   GetWorld()->GetTimerManager().ClearTimer(DefenseHandle);
   SetRunState();
   UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(this,EndDefenseTag, FGameplayEventData());
}

void ABasePlayerCharacter::EndDefense()
{
    if (CharacterState == ECharacterState::Defend || CharacterState == ECharacterState::Block)
        SetCharacterState(ECharacterState::Idle);
} 

void ABasePlayerCharacter::SetWalkState()
{
    GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
}

void ABasePlayerCharacter::SetRunState()
{
    GetCharacterMovement()->MaxWalkSpeed = RunSpeed;
}

void ABasePlayerCharacter::OnDamaged(float DamageAmount, const FHitResult& HitInfo, const struct FGameplayTagContainer& DamageTags, ABaseCharacter* InstigatorCharacter, AActor* DamageCauser)
{
    Super::OnDamaged(DamageAmount,HitInfo,DamageTags,InstigatorCharacter,DamageCauser);

    FVector TraceDirection = (HitInfo.TraceStart - HitInfo.TraceEnd).GetSafeNormal();
    FVector AttackDirection(TraceDirection.X, TraceDirection.Y, 0.0f);

    //CurrentHitDirection=-TraceDirection;

    //SetBodySimulate(HitInfo.BoneName);

    if(bIsSuperArmor)
      return;

    if(!InstigatorCharacter->GetCurrentAttackInfo().bCanHitMontage)
       return;

    if(CharacterState==ECharacterState::LoseBlance||CharacterState==ECharacterState::Dead)
       return;

    PlayHitMontage(GetHitDirection(AttackDirection));
   
}

void ABasePlayerCharacter::OnBlanceChanged(float DeltaValue, const struct FGameplayTagContainer& EventTags)
{
    Super::OnBlanceChanged(DeltaValue,EventTags);
}

FName ABasePlayerCharacter::GetHitDirection(FVector AttackDirection)
{
    float DotValue = FVector::DotProduct(AttackDirection, GetActorForwardVector());
    float CrossValue=FVector::CrossProduct(AttackDirection,GetActorForwardVector()).Z;

    FName Directon;
    if(DotValue>0.86f)
      Directon=TEXT("F");
    else if(DotValue<-0.7f)
      Directon=TEXT("B");
    else if (DotValue > 0.35f)
    {
       if(CrossValue>0.0f)
         Directon=TEXT("FL");
       else
         Directon=TEXT("FR");
    }
    else
    {
        if(CrossValue>0.0f)
           Directon=TEXT("L");
        else
           Directon=TEXT("R");
    }

    return Directon;
}

void ABasePlayerCharacter::Tick(float DeltaTime)
{
   Super::Tick(DeltaTime);

   DeltaTimes=DeltaTime;
   
   GEngine->AddOnScreenDebugMessage(-1,0.0f,FColor::Cyan,GetName()+": "+StaticEnum<ECharacterState>()->GetNameStringByValue((int64)CharacterState));
   GEngine->AddOnScreenDebugMessage(-1,0.0f,FColor::Red,GetName()+": "+FString::SanitizeFloat(CurrentDamageAccumulation));
} 

void ABasePlayerCharacter::BlockOrDefenseTarget(ABaseCharacter* Target, const FHitResult& OutHit, bool bBlock)
{
    FGameplayEventData GameplayEventData;
    if (bBlock)
    {
        GameplayEventData.Instigator = this;
        GameplayEventData.Target = Target;
    }
    else
    {
        GameplayEventData.Instigator = Target;
        GameplayEventData.Target = this;
    }
    
    FGameplayEffectContextHandle EffectContext =GetAbilitySystemComponent()->MakeEffectContext();
    EffectContext.AddHitResult(OutHit);
    GameplayEventData.ContextHandle = EffectContext;
    UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(this,bBlock?BlockHitTag:DefendHitTag,GameplayEventData);
}

void ABasePlayerCharacter::SetCharacterState(ECharacterState NewState)
{
    const ECharacterState& BeforeCharacterState=CharacterState;

    if ((CharacterState == ECharacterState::Defend || CharacterState == ECharacterState::Block))
    {
       if(NewState!=ECharacterState::Defend&&NewState!=ECharacterState::Block)
               CancelDefense();
    }
         
    Super::SetCharacterState(NewState);

    if (BeforeCharacterState == ECharacterState::Idle && bIsTurningToDefense)
    {
        bIsTurningToDefense = false;
        GetWorldTimerManager().ClearTimer(StartBlockHandle);
    }
}
