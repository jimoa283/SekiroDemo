// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/PlayerCharacter.h"
#include "GameFramework\SpringArmComponent.h"
#include "Camera\CameraComponent.h"
#include "GameFramework\CharacterMovementComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Character/MyPlayerController.h"
#include "Character/PlayerLikEnemyCharacter.h"
#include "Kismet/KismetMathLibrary.h"
#include "Components/WidgetComponent.h"
#include "UI/DangerWidget.h"
#include "UI/PlayerHUD.h"
#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Character/BeArmedInterface.h"
#include "UI/ShowBackEnemyWidget.h"
#include "UI/PlayerWidget.h"
#include "Character/GrappleComponent.h"
#include "CableComponent.h"
#include "Character/WallHugComponent.h"
#include "Character/InventoryComponent.h"
#include "Character/HangLedgeComponent.h"
#include "Components/ArrowComponent.h"
#include "Character/InputBufferComponent.h"
#include "Character/InventoryComponent.h"
#include "Item/RPGItem.h"
#include "Ability/RPGGameplayAbility.h"
#include "Kismet/GameplayStatics.h"

APlayerCharacter::APlayerCharacter()
{
     CameraBoom= CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
     CameraBoom->SetupAttachment(RootComponent);

     FollowCamera=CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
     FollowCamera->SetupAttachment(CameraBoom);

     DangerWidget=CreateDefaultSubobject<UWidgetComponent>(TEXT("DangerWidget"));
     DangerWidget->SetupAttachment(GetMesh(),TEXT("head"));

     ExecuteCheckRange=CreateDefaultSubobject<UBoxComponent>(TEXT("ExecuteCheckRange"));
     ExecuteCheckRange->SetupAttachment(RootComponent);

     JumpTwiceCheckRange=CreateDefaultSubobject<UBoxComponent>(TEXT("JumpTwiceCheckRange"));
     JumpTwiceCheckRange->SetupAttachment(RootComponent);

     ShowBackEnemyWidget=CreateDefaultSubobject<UWidgetComponent>(TEXT("ShowBackEnemyWidget"));
     ShowBackEnemyWidget->SetupAttachment(GetMesh(),TEXT("spine_03"));

     Rope=CreateDefaultSubobject<UCableComponent>(TEXT("Rope"));
     Rope->SetupAttachment(GetMesh(),TEXT("hand_l"));

     Kunai=CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Kunai"));
     Kunai->SetupAttachment(RootComponent);

     LeftHangArrow=CreateDefaultSubobject<UArrowComponent>(TEXT("LeftHangArrow"));
     LeftHangArrow->SetupAttachment(RootComponent);

     RightHangArrow = CreateDefaultSubobject<UArrowComponent>(TEXT("RightHangArrow"));
     RightHangArrow->SetupAttachment(RootComponent);

     GrappleComponent=CreateDefaultSubobject<UGrappleComponent>(TEXT("GrappleComponent"));

     WallHugComponent=CreateDefaultSubobject<UWallHugComponent>(TEXT("WallHugComponent"));

     HangLedgeComponent=CreateDefaultSubobject<UHangLedgeComponent>(TEXT("HangLedgeComponent"));

     Inventory=CreateDefaultSubobject<UInventoryComponent>(TEXT("Inventory"));

     //InputBufferComponent=CreateDefaultSubobject<UInputBufferComponent>(TEXT("InputBufferComponent"));

     TryToArmRange=600.0f;
     TryToArmDistance=2000.0f;
     BlockTime=0.1f;

}

void APlayerCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    PlayerInputComponent->BindAxis("LookUp", this, &APlayerCharacter::LookUp);
    PlayerInputComponent->BindAxis("LookRight", this, &APlayerCharacter::LookRight);

    PlayerInputComponent->BindAxis("MoveForward",this,&APlayerCharacter::DoMoveForward);
    PlayerInputComponent->BindAxis("MoveRight",this,&APlayerCharacter::DoMoveRight);
    PlayerInputComponent->BindAxis("LookUp",this,&APlayerCharacter::LookUp);
    PlayerInputComponent->BindAxis("LookRight",this,&APlayerCharacter::LookRight);

    PlayerInputComponent->BindAction("SetWalkState",IE_Pressed,this,&APlayerCharacter::SetWalkState);
    PlayerInputComponent->BindAction("SetWalkState",IE_Released,this,&APlayerCharacter::SetRunState);

    PlayerInputComponent->BindAction("Crouch",IE_Pressed,this,&APlayerCharacter::DoCrouch);
    PlayerInputComponent->BindAction("Jump",IE_Pressed,this,&APlayerCharacter::DoJump);
    //PlayerInputComponent->BindAction("Melee",IE_Pressed,this,&APlayerCharacter::DoAttack);
    PlayerInputComponent->BindAction("Arm",IE_Pressed,this,&APlayerCharacter::DoArm);
    PlayerInputComponent->BindAction("ResetView",IE_Pressed,this,&APlayerCharacter::ResetView);
    PlayerInputComponent->BindAction("Avoid",IE_Pressed,this,&APlayerCharacter::DoAvoid);
    PlayerInputComponent->BindAction("Defense",IE_Pressed,this,&APlayerCharacter::StartBlock);
    PlayerInputComponent->BindAction("Defense",IE_Released,this,&APlayerCharacter::EndDefense);
    PlayerInputComponent->BindAction("Grapple",IE_Pressed,this,&APlayerCharacter::DoGrapple);
    PlayerInputComponent->BindAction("Action",IE_Pressed,this,&APlayerCharacter::DoWallHug);
    PlayerInputComponent->BindAction("Action",IE_Pressed,this,&APlayerCharacter::DoClimbLedge);
    PlayerInputComponent->BindAction("Avoid",IE_Pressed,this,&APlayerCharacter::DoCancelHang);
    PlayerInputComponent->BindAction("Action",IE_Pressed,this,&APlayerCharacter::DoDropToLedge);
    PlayerInputComponent->BindAction("ToggleInventory",IE_Pressed,this,&APlayerCharacter::ToggleInventoryWidget);
    PlayerInputComponent->BindAction("UseArtificialHand",IE_Pressed,this,&APlayerCharacter::UseArtificialHand);
    PlayerInputComponent->BindAction("ChangeItemUp",IE_Pressed,this,&APlayerCharacter::ChangeToUpUseItem);
    PlayerInputComponent->BindAction("ChangeItemDown",IE_Pressed,this,&APlayerCharacter::ChangeToDownUseItem);
    PlayerInputComponent->BindAction("UseItem",IE_Pressed,this,&APlayerCharacter::UseItem);
    PlayerInputComponent->BindAction("ChangeArtificialHand",IE_Pressed,this,&APlayerCharacter::ChangeAritficialHand);
}

void APlayerCharacter::BeginPlay()
{
   Super::BeginPlay();

   LandedDelegate.AddDynamic(this,&APlayerCharacter::HandleLanded);

   WBPDanger=Cast<UDangerWidget>(DangerWidget->GetWidget());
   if(WBPDanger)
     WBPDanger->OnUpdateCompleted.AddDynamic(this,&APlayerCharacter::HideDanger);

   WBPShowBackEnemy=Cast<UShowBackEnemyWidget>(ShowBackEnemyWidget->GetWidget());

   JumpTwiceCheckRange->OnComponentBeginOverlap.AddDynamic(this,&APlayerCharacter::JumpTwiceOnTarget);

   WallHugComponent->OnWallHugEnd.AddDynamic(this,&APlayerCharacter::OnWallHugEnd);

   if (WBPPlayerHUD)
   {
       CharacterHUD = CreateWidget<UPlayerHUD>(GetWorld(),WBPPlayerHUD);
       CharacterHUD->AddToViewport();
   }

   if (WBPInventory)
   {
       InventoryWidget=CreateWidget<UUserWidget>(GetWorld(),WBPInventory);
   }
      
   HideDanger();

   bIsEquip=true;
}



void APlayerCharacter::LookUp(float AxisValue)
{
   if(!IsArmed)
   AddControllerPitchInput(AxisValue);
}

void APlayerCharacter::LookRight(float AxisValue)
{
   if(!IsArmed)
   AddControllerYawInput(AxisValue);
}


void APlayerCharacter::DoMoveForward(float AxisValue)
{
   if(!IsAlive())
      return;

   if(CharacterState==ECharacterState::WallHug)
     DoWallHugMove(AxisValue,false);
   else if(CharacterState==ECharacterState::Grapple)
      return;
   else if(CharacterState==ECharacterState::HangLedge)
      return;
   else
     MoveForward(AxisValue);
}

void APlayerCharacter::DoMoveRight(float AxisValue)
{
    if(!IsAlive())
      return;

    if(CharacterState==ECharacterState::WallHug)
      DoWallHugMove(AxisValue,true);
    else if(CharacterState==ECharacterState::HangLedge)
       HangLedgeComponent->HandleMoveInput(AxisValue);
    else if(CharacterState==ECharacterState::Grapple)
       return;
    else
      MoveRight(AxisValue);
}

void APlayerCharacter::DoWallHugMove(float Scale, bool Right)
{
   WallHugComponent->HandleMovementInput(Scale,Right);
}

void APlayerCharacter::OnWallHugEnd()
{
    SetCharacterState(ECharacterState::Idle);
}

bool APlayerCharacter::ActivateAbilitiesWithItem(const FGameplayAbilitySpecHandle& ItemAbility,bool bAllowRemoteActivation)
{
    if(!ItemAbility.IsValid())
       return false;

    if (AbilitySystemComponent)
    {
        return AbilitySystemComponent->TryActivateAbility(ItemAbility,bAllowRemoteActivation);
    }

    return false;
}

void APlayerCharacter::ToDead()
{
    PlayAnimMontage(DeadMontage);

    Super::ToDead();
 
}

void APlayerCharacter::StartCommand(FString Command)
{
   if(Command=="Attack")
       DoAttack();
   else if(Command=="MoveForward")
     CanMoveForward=true;
   else if(Command=="EndMoveForward")
     CanMoveForward=false;
   else if(Command=="MoveBack")
     CanMoveBack=true;
   else if(Command=="EndMoveBack")
     CanMoveBack=false;
   else if(Command=="MoveRight")
     CanMoveRight=true;
   else if(Command=="EndMoveRight")
     CanMoveRight=false;
   else if(Command=="MoveLeft")
     CanMoveLeft=true;
   else if(Command=="EndMoveLeft")
     CanMoveLeft=false;
   else if(Command=="Jump")
     DoJump();
   else if(Command=="Crouch")
     DoCrouch();
   else if(Command=="Arm")
     DoArm();
   else if (Command == "Avoid")
   {       
       DoAvoid();
       DoCancelHang();
   }   
   else if(Command=="StartBlock")
     StartBlock();
   else if(Command=="EndDefense")
     EndDefense();
   else if(Command=="Grapple")
     DoGrapple();
   else if (Command == "Action")
   {
       DoWallHug();
       DoClimbLedge();
       DoDropToLedge();
   }
}

void APlayerCharacter::DoAttack()
{

   if(CharacterState != ECharacterState::Attack &&CharacterState!=ECharacterState::Idle)
        return;

   if(DoExecute())
      return;

   DoMeleeAttack();
}

bool APlayerCharacter::DoExecute()
{
    if(!BeArmedTarget)
       return false;
    
    TArray<AActor*> Temp;
    ExecuteCheckRange->GetOverlappingActors(Temp);

    if(!Temp.Contains(BeArmedTarget))
       return false;

    if (BeArmedTarget->GetArmTarget() != nullptr)
    {
        if (BeArmedTarget->GetCharacterState() != ECharacterState::LoseBlance)
        {
            return false;
        }
    }

    if (UAnimMontage* Montage = GetMesh()->GetAnimInstance()->GetCurrentActiveMontage())
    {
        StopAnimMontage(GetMesh()->GetAnimInstance()->GetCurrentActiveMontage());
    }

    GetWorldTimerManager().ClearTimer(TimeDownHandle);
    UGameplayStatics::SetGlobalTimeDilation(this, 1.f);

    float Angle = (180.0f / PI) * FMath::Acos(FVector::DotProduct(GetActorForwardVector(), BeArmedTarget->GetActorForwardVector()));

    bool IsForward = 90 < Angle && Angle <= 180.0f;

    const FExeucteInfo& ExecuteInfo = IsForward ? ForwardExecuteInfo : BackExecuteInfo;
    BeArmedTarget->SetActorLocation(GetActorLocation() + GetActorForwardVector() * ExecuteInfo.TargetDistance);

    FRotator TaragetRotation= IsForward ? UKismetMathLibrary::MakeRotFromX(-GetActorForwardVector()) : UKismetMathLibrary::MakeRotFromX(GetActorForwardVector());
    BeArmedTarget->GetController()->SetControlRotation(TaragetRotation);

    ActivateAbilitiesWithTags(ExecuteInfo.ExecuteTag);
    BeArmedTarget->BeExecuted(ExecuteInfo.TargetMontage);

    return true;

}


void APlayerCharacter::DoSpecificExecute()
{
   
}

void APlayerCharacter::DoCrouch()
{
    if(CharacterState != ECharacterState::Idle)
       return;
    
    if (bIsCrouched)
    {
        CharacterState=ECharacterState::Idle;
        UnCrouch();
    }     
    else
    {
        CharacterState=ECharacterState::Crouch;
        Crouch();
    }
}

void APlayerCharacter::DoJump()
{

    if(CharacterState!=ECharacterState::Idle)
       return;

   SetCharacterState(ECharacterState::Jump);
   ActivateAbilitiesWithTags(JumpTag);
}

void APlayerCharacter::DoGrapple()
{
   if(CharacterState!=ECharacterState::Idle&&CharacterState!=ECharacterState::Jump)
      return;
    
    GrappleComponent->Grapple();
}

void APlayerCharacter::DoWallHug()
{ 

    if (CharacterState == ECharacterState::Idle)
    {
        if(!BeArmedTarget&&!GetCharacterMovement()->IsFalling() && WallHugComponent->TryBeginWallHug())
          SetCharacterState(ECharacterState::WallHug);
    }
    else if (CharacterState == ECharacterState::WallHug)
    {
        WallHugComponent->EndWallHug();
    }
   
}

void APlayerCharacter::DoClimbLedge()
{ 
   if(CharacterState!=ECharacterState::HangLedge)
      return;

   HangLedgeComponent->DoClimbLedge();
}

void APlayerCharacter::DoCancelHang()
{
   if(CharacterState!=ECharacterState::HangLedge)
      return;

    HangLedgeComponent->CancelHang();
}

void APlayerCharacter::DoDropToLedge()
{
   if(CharacterState!=ECharacterState::Idle||BeArmedTarget)
      return;

   if(HangLedgeComponent->TryToDropToLedge())
      SetCharacterState(ECharacterState::HangLedge);
}

void APlayerCharacter::UseItem()
{  
  if(CharacterState!=ECharacterState::Idle||!Inventory->CanUseCurrentItem())
        return;

  ActivateAbilitiesWithItem(ItemAbilityMap[URPGAssetManager::PotionItemType]);
}

void APlayerCharacter::UseSkill()
{
   if(CharacterState!=ECharacterState::Defend&&CharacterState!=ECharacterState::Block)
      return;
   
   ActivateAbilitiesWithItem(ItemAbilityMap[URPGAssetManager::SkillItemType]);
}

void APlayerCharacter::UseArtificialHand()
{
    if (CharacterState != ECharacterState::Idle)
        return;

  ActivateAbilitiesWithItem(ItemAbilityMap[URPGAssetManager::WeaponItemType]);
}

void APlayerCharacter::ChangeToUpUseItem()
{
   Inventory->ChangeToOtherEquipItem(URPGAssetManager::PotionItemType,true);
}

void APlayerCharacter::ChangeToDownUseItem()
{
   Inventory->ChangeToOtherEquipItem(URPGAssetManager::PotionItemType,false);
}

void APlayerCharacter::ChangeAritficialHand()
{
   Inventory->ChangeToOtherEquipItem(URPGAssetManager::WeaponItemType,false);
}

void APlayerCharacter::ToggleInventoryWidget()
{
   if(!InventoryWidget)
      return;

   //if (InventoryWidget->IsInViewport())
   //{
   //   InventoryWidget->RemoveFromViewport();
   //   APlayerController* PlayerController= Cast<APlayerController>(GetController());
   //   PlayerController->SetInputMode(FInputModeGameOnly());
   //   PlayerController->bShowMouseCursor=false;
   //}
   //else
   //{
      InventoryWidget->AddToViewport();
      APlayerController* PlayerController = Cast<APlayerController>(GetController());
      PlayerController->SetInputMode(FInputModeUIOnly());
      PlayerController->bShowMouseCursor = true;
   //}
}

void APlayerCharacter::JumpTwiceOnTarget(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
  GEngine->AddOnScreenDebugMessage(-1,3.0f,FColor::Red,OtherActor->GetName());
  
   if(CharacterState!=ECharacterState::Jump)
     return;


   if(HasJumpTwice)
        return;


   if(!OtherActor->ActorHasTag("Enemy"))
      return;

   HasJumpTwice = true;

   FGameplayEventData GameplayEventData;
   GameplayEventData.Instigator = this;
   GameplayEventData.Target = OtherActor;
   FGameplayEffectContextHandle EffectContext = GetAbilitySystemComponent()->MakeEffectContext();
   EffectContext.AddHitResult(SweepResult);
   GameplayEventData.ContextHandle = EffectContext;
   UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(this, JumpTwiceTag, GameplayEventData);
}

void APlayerCharacter::HandleLanded(const FHitResult& Hit)
{
    HasJumpTwice = false;
    SetCharacterState(ECharacterState::Idle);
    UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(this, EndJumpTag, FGameplayEventData());   
}

void APlayerCharacter::UpdateShowBackEnemy()
{
    FVector ForwardVector = UKismetMathLibrary::GetForwardVector(GetControlRotation());
    FVector PlayerLocation=GetActorLocation();
    for (TPair<ABaseCharacter*,FShowBackEnemyInfo>& Pair : ShowBackEnemyMap)
    {
       FVector TargetVector= Pair.Key->GetActorLocation() - PlayerLocation;
       float Angle= (180.0f / PI) * FMath::Acos(FVector::DotProduct(ForwardVector, (TargetVector).GetSafeNormal2D()));
       float CrossZ=FVector::CrossProduct(ForwardVector,TargetVector).Z;
       Pair.Value.Angle=Angle;
       Pair.Value.IsLeft=CrossZ>0;
    }

    if(WBPShowBackEnemy)
      WBPShowBackEnemy->UpdateShowBackEnemyArrow(ShowBackEnemyMap);
}

void APlayerCharacter::DoArm()
{
   if(IsArmed)
     CancelArm();
   else
   {
       GetWorld()->GetTimerManager().SetTimer(UpdateArmHandle, this, &APlayerCharacter::TryToArm, UpdateArmTime, true);
   }
     
}

void APlayerCharacter::TryToArm()
{
   IsArmed=true;
    
   FVector StartLocation=GetActorLocation();
   FVector ForwardVector=UKismetMathLibrary::GetForwardVector(GetControlRotation());
   FVector EndLocation=StartLocation+ForwardVector*TryToArmDistance;

   TArray<TEnumAsByte<EObjectTypeQuery> > ObjectTypes;
   ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_Pawn));

   TArray<FHitResult> ArmTargets;

   AActor* CurrentActor=nullptr;
   ABaseCharacter* CurrentCharacter;
   if (UKismetSystemLibrary::CapsuleTraceMultiForObjects(this, StartLocation, EndLocation, TryToArmRange, 200.0f, ObjectTypes, false, TArray<AActor*>(), EDrawDebugTrace::None,
       ArmTargets, true))
   {
       TryToArmTagets.Empty();

       for (const FHitResult& Hit : ArmTargets)
       {
           CurrentActor=Hit.GetActor();
           if(!CurrentActor->ActorHasTag(TEXT("Enemy")))
             continue;

           CurrentCharacter=Cast<ABaseCharacter>(CurrentActor);

           if(!CurrentCharacter->IsAlive())
              continue;

           TryToArmTagets.AddUnique(CurrentCharacter);
       }

       if (TryToArmTagets.Num() == 0)
       {
           GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Red, "No Targets");
           CancelArm();
           return;
       }
         

       float BestAngle=0.0f;
       ABaseCharacter* BestTarget=nullptr;
       float CurrentAngle;

       for (auto Target : TryToArmTagets)
       {
           CurrentAngle=(180.0f/PI)*FMath::Acos(FVector::DotProduct(ForwardVector,(Target->GetActorLocation()-GetActorLocation()).GetSafeNormal2D()));
           
           if (CurrentAngle < BestAngle || BestTarget == nullptr)
           {
              CurrentAngle=BestAngle;
              BestTarget=Target;
           }
       }

       if (BeArmedTarget != BestTarget)
       {
           if (BeArmedTarget)
           { 
               IBeArmedInterface* Interface1=Cast<IBeArmedInterface>(BeArmedTarget);
               Interface1->CancelBeArmed();
               Interface1->HideCanbeExecute();
           }

           BeArmedTarget = BestTarget;

           Cast<IBeArmedInterface>(BeArmedTarget)->SetBeArmed();

           bUseControllerRotationYaw = true;
       }

       if(!BeArmedTarget)
         return;

       IBeArmedInterface* TargetInterface = Cast<IBeArmedInterface>(BeArmedTarget);

       TArray<AActor*> Temp;
       ExecuteCheckRange->GetOverlappingActors(Temp);

       bool bIsShowing=TargetInterface->IsShowing();
       
       if (!Temp.Contains(BeArmedTarget))
       {
           if(bIsShowing)
             TargetInterface->HideCanbeExecute();
       }
       else
       {
          if(!bIsShowing&&TargetInterface->CanShow())
            TargetInterface->ShowCanBeExecute();
       }
        
   }
   else
   {
      GEngine->AddOnScreenDebugMessage(-1,3.0f,FColor::Red,"No Hits");
       CancelArm();
   }
}

void APlayerCharacter::CancelArm()
{
   GetWorld()->GetTimerManager().ClearTimer(UpdateArmHandle);
   bUseControllerRotationYaw=false;
   IsArmed=false;
   if (BeArmedTarget)
   {
       IBeArmedInterface* Inteface=Cast<IBeArmedInterface>(BeArmedTarget);
       Inteface->CancelBeArmed();
       if(Inteface->IsShowing())
         Inteface->HideCanbeExecute();
       BeArmedTarget = nullptr;
   }
   
}

void APlayerCharacter::ResetView()
{
   if(IsArmed)
     return;

   FRotator Rotation=GetActorRotation();
   GetController()->SetControlRotation(Rotation);
}


void APlayerCharacter::Tick(float DeltaTime)
{
   Super::Tick(DeltaTime);

   UpdateShowBackEnemy();

   GEngine->AddOnScreenDebugMessage(-1,0.0f,FColor::Green,"WalkSpeed: "+ FString::SanitizeFloat(GetCharacterMovement()->MaxWalkSpeed));
}

FName APlayerCharacter::GetDirection()
{
   FName Direction;
    if (!IsArmed)
    {
        Direction = "Forward";
    }
    else
    {
       if(MoveAxis.Y>0||(MoveAxis.X==0&&MoveAxis.Y==0))
         Direction="Forward";
       else if(MoveAxis.Y<0)
         Direction="Back";
       else if(MoveAxis.X>0)
         Direction="Right";
       else
         Direction="Left";
    }

    return Direction;
}

void APlayerCharacter::ShowDanger()
{
    DangerWidget->SetHiddenInGame(false);

    if(WBPDanger)
      WBPDanger->UpdateImage();
}

void APlayerCharacter::HideDanger()
{
   DangerWidget->SetHiddenInGame(true);
}

void APlayerCharacter::AddNewShowBackEnemy(ABaseCharacter* Target)
{
   if(ShowBackEnemyMap.Contains(Target))
     return;

   ShowBackEnemyMap.Add(Target,FShowBackEnemyInfo());

   if(WBPShowBackEnemy)
      WBPShowBackEnemy->AddShowBackEnemyInfo(Target);

   if(ShowBackEnemyMap.Num()==1)
     ShowBackEnemyWidget->SetHiddenInGame(false);
}

void APlayerCharacter::RemoveShowBackEnemy(ABaseCharacter* Target)
{
   if(!ShowBackEnemyMap.Contains(Target))
      return;
   
   ShowBackEnemyMap.Remove(Target);
   if(WBPShowBackEnemy)
     WBPShowBackEnemy->RemoveShowBackEnemyInfo(Target);

   if(ShowBackEnemyMap.Num()==0)
     ShowBackEnemyWidget->SetHiddenInGame(true);
}

void APlayerCharacter::AddMoney(int32 DeltaValue)
{  
   Inventory->AddMoney(DeltaValue);
}

UCableComponent* APlayerCharacter::GetRope() const
{
  return Rope;
}

FVector APlayerCharacter::GetLeftHangArrowLocation() const
{
   return LeftHangArrow->GetComponentLocation();
}

FVector APlayerCharacter::GetRightHangArrowLocation() const
{
  return RightHangArrow->GetComponentLocation();
}

void APlayerCharacter::ChangeEquipItem(URPGItem* ChangeItem, FPrimaryAssetType ChangeItemType)
{ 
    if (ItemAbilityMap.Contains(ChangeItemType))
    {
        FGameplayAbilitySpecHandle* FoundHandle=ItemAbilityMap.Find(ChangeItemType);
        
        if (FoundHandle && AbilitySystemComponent&& 
                AbilitySystemComponent->FindAbilitySpecFromHandle(ItemAbilityMap[ChangeItemType]))
        {
            AbilitySystemComponent->ClearAbility(ItemAbilityMap[ChangeItemType]);
        }
        
        if (ChangeItem)
        {
            ItemAbilityMap[ChangeItemType]=AbilitySystemComponent->GiveAbility(FGameplayAbilitySpec(ChangeItem->ItemAbility, GetCharacterLevel(), INDEX_NONE, ChangeItem));
        }
    }
}

void APlayerCharacter::TargetLoseBlanceHitPause(ABaseCharacter* Target)
{
   if(Target!=BeArmedTarget)
      return;

   TArray<AActor*> Temp;
   ExecuteCheckRange->GetOverlappingActors(Temp);

   if(!Temp.Contains(Target))
     return;
   
   float Angle = (180.0f / PI) * FMath::Acos(FVector::DotProduct(GetActorForwardVector(), Target->GetActorForwardVector()));

   bool IsForward = 90 < Angle && Angle <= 180.0f;

   if(!IsForward)
      return;

    TargetLength=TargetLoseBlanceLength;
    TargetOffset=TargetLoseBlanceOffset;
    UpdateSpeed=TargetLoseBlanceUpdateSpeed;

   GetWorldTimerManager().SetTimer(UpdateCameraPositionHandle,this,&APlayerCharacter::UpdateTargetLoseBlanceCamera,DeltaTimes,true);
   GetWorldTimerManager().SetTimer(TimeDownHandle,this,&APlayerCharacter::StartTargetLoseBlanceTimeDown,TimeDownWaitTime,false);
}

void APlayerCharacter::UpdateTargetLoseBlanceCamera()
{
    CameraBoom->TargetArmLength = UKismetMathLibrary::FInterpTo(CameraBoom->TargetArmLength, TargetLength, DeltaTimes, UpdateSpeed);
    CameraBoom->SocketOffset = UKismetMathLibrary::VInterpTo(CameraBoom->SocketOffset,TargetOffset, DeltaTimes, UpdateSpeed);

    if (UKismetMathLibrary::NearlyEqual_FloatFloat(CameraBoom->TargetArmLength,TargetLength, 0.1f))
    {
        GetWorldTimerManager().ClearTimer(UpdateCameraPositionHandle);
    }
}

void APlayerCharacter::StartTargetLoseBlanceTimeDown()
{
   UGameplayStatics::SetGlobalTimeDilation(this,TimeDownValue);

   GetWorldTimerManager().SetTimer(TimeDownHandle,this,&APlayerCharacter::EndTargetLoseBlanceTimeDown,TimeDownTime,false);
}

void APlayerCharacter::EndTargetLoseBlanceTimeDown()
{ 
    GetWorldTimerManager().ClearTimer(TimeDownHandle);
    UGameplayStatics::SetGlobalTimeDilation(this, 1.f);
    ReturnDefaultCameraPosition();
}

void APlayerCharacter::ReturnDefaultCameraPosition()
{
    TargetLength=DefaultLength;
    TargetOffset=DefaultOffset;
    UpdateSpeed=ReturnDefaultUpdateSpeed;

    GetWorldTimerManager().SetTimer(UpdateCameraPositionHandle,this,&APlayerCharacter::UpdateTargetLoseBlanceCamera,DeltaTimes,true);
}





