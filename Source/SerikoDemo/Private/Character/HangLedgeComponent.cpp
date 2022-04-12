// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/HangLedgeComponent.h"
#include "Character/PlayerCharacter.h"
#include "Kismet/KismetSystemLibrary.h"
#include "SerikoDemo/SerikoDemo.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"

// Sets default values for this component's properties
UHangLedgeComponent::UHangLedgeComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UHangLedgeComponent::BeginPlay()
{
	Super::BeginPlay();

	PlayerCharacter=GetOwner<APlayerCharacter>();
	
}


void UHangLedgeComponent::ForwardTrace()
{
    FVector Start=PlayerCharacter->GetActorLocation();
	FVector ForwardVector=PlayerCharacter->GetActorForwardVector();
	FVector End=Start+FVector(ForwardVector.X*150.0f,ForwardVector.Y*150.0f,ForwardVector.Z);

	if(!UKismetSystemLibrary::SphereTraceSingle(this,Start,End,20.0f,UEngineTypes::ConvertToTraceType(WALLTRACE),false,TArray<AActor*>(),EDrawDebugTrace::ForOneFrame,ForwardTraceOutHit,true))
	    return;

    JumpWallLocation=ForwardTraceOutHit.Location;
	JumpWallNormal=ForwardTraceOutHit.Normal;
}

void UHangLedgeComponent::HeightTrace()
{
   FVector ForwardVector=PlayerCharacter->GetActorForwardVector();
   FVector CharacterLocation=PlayerCharacter->GetActorLocation();
   FVector Start=FVector(CharacterLocation.X,CharacterLocation.Y,CharacterLocation.Z+500.0f)+ForwardVector*70.0f;
   FVector End=FVector(Start.X,Start.Y,Start.Z-500.0f);

   if (!UKismetSystemLibrary::SphereTraceSingle(this, Start, End, 20.0f, UEngineTypes::ConvertToTraceType(WALLTRACE), false, TArray<AActor*>(), EDrawDebugTrace::ForOneFrame,HeightTraceOutHit,true))
	   return;

   JumpHeightLocation=HeightTraceOutHit.Location;

   float HeightOffset=PlayerCharacter->GetMesh()->GetSocketLocation(TEXT("PelvisSocket")).Z-JumpHeightLocation.Z;
   if(!UKismetMathLibrary::InRange_FloatFloat(HeightOffset,-50.0f,0.0f))
       return;
   
   if(HangLedgeState!=EHangLedgeState::ClimbingLedge)
       GrabLedge();
}

void UHangLedgeComponent::LeftTrace()
{
   FVector LeftLocation=PlayerCharacter->GetLeftHangArrowLocation();
   CanMoveLeft=UKismetSystemLibrary::CapsuleTraceSingle(this,LeftLocation,LeftLocation,20.0f,60.0f,
                     UEngineTypes::ConvertToTraceType(WALLTRACE),false,TArray<AActor*>(),EDrawDebugTrace::ForOneFrame,LeftTraceOutHit,true);
}

void UHangLedgeComponent::RightTrace()
{
	FVector RightLocation = PlayerCharacter->GetRightHangArrowLocation();
	CanMoveRight = UKismetSystemLibrary::CapsuleTraceSingle(this, RightLocation, RightLocation, 20.0f, 60.0f,
		UEngineTypes::ConvertToTraceType(WALLTRACE), false, TArray<AActor*>(), EDrawDebugTrace::ForOneFrame, RightTraceOutHit, true);
}

void UHangLedgeComponent::GrabLedge()
{
   PlayerCharacter->GetCharacterMovement()->SetMovementMode(MOVE_Flying);
   //PlayerCharacter->GetCharacterMovement()->GravityScale=0.f;
   HangLedgeState=EHangLedgeState::Hanging;
   PlayerCharacter->SetCharacterState(ECharacterState::HangLedge);
   PlayerCharacter->GetCharacterMovement()->bOrientRotationToMovement = false;
   PlayerCharacter->GetCharacterMovement()->MaxFlySpeed=HangMoveSpeed;

   FVector TempLocation=JumpWallNormal*FVector(22.0f,22.0f,0.0f);

   FVector TargetLocation=FVector(TempLocation.X+JumpWallLocation.X,TempLocation.Y+JumpWallLocation.Y,JumpHeightLocation.Z-120.0f);

   FRotator TempRotation=UKismetMathLibrary::MakeRotFromX(JumpWallNormal);

   FRotator TargetRotation=FRotator(TempRotation.Pitch,TempRotation.Yaw-180.0f,TempRotation.Roll);

   FLatentActionInfo ActionInfo;

   ActionInfo.CallbackTarget=this;
   ActionInfo.ExecutionFunction="OnCompleteHang";
   ActionInfo.Linkage=0;

   UKismetSystemLibrary::MoveComponentTo(PlayerCharacter->GetCapsuleComponent(),TargetLocation,TargetRotation,false,false,0.13f,false,EMoveComponentAction::Move,ActionInfo);
}

void UHangLedgeComponent::ForwardDownTrace()
{
	FVector CharacterLocation = PlayerCharacter->GetActorLocation();
	FVector ForwardVector = PlayerCharacter->GetActorForwardVector();

	FVector ForwardDropStart = CharacterLocation + ForwardVector * 70.0f - FVector(0,0,DropTraceStartZ);
	FVector ForwardDropEnd = ForwardDropStart - FVector(0,0,DropTraceHeight);

	CanForwardDrop=!UKismetSystemLibrary::SphereTraceSingle(this,ForwardDropStart,ForwardDropEnd,20.0f,UEngineTypes::ConvertToTraceType(WALLTRACE),
	                                                        false,TArray<AActor*>(),EDrawDebugTrace::ForOneFrame,ForwardDownTraceOutHit,false);
}

void UHangLedgeComponent::BackwardDownTrace()
{
	FVector CharacterLocation = PlayerCharacter->GetActorLocation();
	FVector ForwardVector = PlayerCharacter->GetActorForwardVector();

	FVector BackDropStart = CharacterLocation - ForwardVector * 70.0f - FVector(0, 0, DropTraceStartZ);
	FVector BackDropEnd = BackDropStart - FVector(0, 0, DropTraceHeight);

	CanBackDrop = !UKismetSystemLibrary::SphereTraceSingle(this, BackDropStart, BackDropEnd, 20.0f, UEngineTypes::ConvertToTraceType(WALLTRACE),
		false, TArray<AActor*>(), EDrawDebugTrace::ForOneFrame, BackDownTraceOutHit, false);
}

bool UHangLedgeComponent::FindCloestWall(FVector LineTraceStart, int32 _NumLineTraces, float& _WallDist, FVector& _WallLocation, FVector& _WallNormal)
{
    float ClosestWallDist=0;
	FVector ClosestWallLocation=FVector::ZeroVector;
	FVector ClosestWallNormal=FVector::ZeroVector;

	float CurrentWallDist;
	FVector CurrentWallLocation;
	FVector CurrentWallNormal;

	FHitResult OutHit;
	for (int32 i = 0; i < NumLineTraces - 1; i++)
	{
		if (LineTrace(LineTraceStart, i * 360.0f / _NumLineTraces, OutHit))
		{
			CurrentWallDist=OutHit.Distance;
			CurrentWallLocation = OutHit.Location;
			CurrentWallNormal = OutHit.Normal;

			if (CurrentWallDist > ClosestWallDist || ClosestWallNormal == FVector::ZeroVector)
			{
				ClosestWallDist = CurrentWallDist;
				ClosestWallLocation = CurrentWallLocation;
				ClosestWallNormal = CurrentWallNormal;
			}
	   }
	}

	_WallDist = ClosestWallDist;
	_WallLocation = ClosestWallLocation;
	_WallNormal = ClosestWallNormal;

	return ClosestWallNormal != FVector::ZeroVector;
}

bool UHangLedgeComponent::LineTrace(FVector Start, float RotationAngleFromForward, FHitResult& OutHit)
{
	FVector End = Start + UKismetMathLibrary::RotateAngleAxis(PlayerCharacter->GetActorForwardVector(), RotationAngleFromForward, FVector(0.f, 0.f, 1.f)) * 100;

	return UKismetSystemLibrary::LineTraceSingle(this,End,Start, UEngineTypes::ConvertToTraceType(WALLTRACE), false, TArray<AActor*>(), EDrawDebugTrace::ForDuration, OutHit, true);
}

void UHangLedgeComponent::StartDropToLedge()
{
   HangLedgeState=EHangLedgeState::DropToLedge;
   PlayerCharacter->GetMesh()->GetAnimInstance()->Montage_Play(DropToLedgeMontage);
}

void UHangLedgeComponent::EndDropToLedge()
{
    GrabLedge();
}

void UHangLedgeComponent::OnCompleteHang()
{
	PlayerCharacter->GetCharacterMovement()->StopMovementImmediately();
}

void UHangLedgeComponent::DoClimbLedge()
{
   if(HangLedgeState!=EHangLedgeState::Hanging)
      return;
   
   HangLedgeState=EHangLedgeState::ClimbingLedge;
   PlayerCharacter->GetMesh()->GetAnimInstance()->Montage_Play(ClimbLedgeMontage);
}

void UHangLedgeComponent::EndClimbLedge()
{
   if(HangLedgeState!=EHangLedgeState::ClimbingLedge)
      return;
  
   HangLedgeState=EHangLedgeState::None;
   PlayerCharacter->SetCharacterState(ECharacterState::Idle); 
   PlayerCharacter->Landed(FHitResult());
   PlayerCharacter->GetCharacterMovement()->SetMovementMode(MOVE_Walking);
   PlayerCharacter->GetCharacterMovement()->MaxFlySpeed = 600.0f;
   PlayerCharacter->GetCharacterMovement()->bOrientRotationToMovement = true;
}

void UHangLedgeComponent::TickTurnToLedge(float DeltaSeconds)
{
   MoveToLedgeTime=FMath::Clamp(DeltaSeconds/MoveToLedgeDuration+MoveToLedgeTime,0.0f,1.0f);
   PlayerCharacter->SetActorLocation(FMath::Lerp(MoveToLedgeBegin,MoveToLedgeEnd,MoveToLedgeTime),true);
   PlayerCharacter->SetActorRotation(FMath::Lerp(MoveToLedgeRotBegin,MoveToLedgeRotEnd,MoveToLedgeTime));

   if (MoveToLedgeTime >= 1.0f)
   {
	   StartDropToLedge();
   }
}

void UHangLedgeComponent::CancelHang()
{
   if(HangLedgeState!=EHangLedgeState::Hanging)
     return;
   
   HangLedgeState=EHangLedgeState::None;
   PlayerCharacter->SetCharacterState(ECharacterState::Jump);
   PlayerCharacter->GetCharacterMovement()->SetMovementMode(MOVE_Walking);
   PlayerCharacter->GetCharacterMovement()->MaxFlySpeed = 600.0f;
   PlayerCharacter->GetCharacterMovement()->bOrientRotationToMovement = true;
}

bool UHangLedgeComponent::TryToDropToLedge()
{
   if(HangLedgeState!=EHangLedgeState::None)
      return false;
    
   if(!CanBackDrop&&!CanForwardDrop)
      return false;

   float _WallDist;
   FVector _WallLocation;
   FVector _WallNormal;
   if (FindCloestWall(PlayerCharacter->GetActorLocation() - FVector(0.f, 0.f, FindWallTraceStartZ), NumLineTraces, _WallDist, _WallLocation, _WallNormal))
   {
       MoveToLedgeBegin=PlayerCharacter->GetActorLocation();
	   MoveToLedgeEnd=FVector(_WallLocation.X,_WallLocation.Y,MoveToLedgeBegin.Z)-_WallNormal*DistanceToWall;
	   MoveToLedgeRotBegin=PlayerCharacter->GetActorRotation();
	   MoveToLedgeRotEnd= UKismetMathLibrary::MakeRotFromZX(FVector::UpVector, -_WallNormal);
	   MoveToLedgeTime=0.f;
	   HangLedgeState=EHangLedgeState::TurnToDrop;
	   PlayerCharacter->GetCharacterMovement()->bOrientRotationToMovement = false;
	   PlayerCharacter->GetCharacterMovement()->MaxFlySpeed = 100.0f;
	   PlayerCharacter->GetCharacterMovement()->SetMovementMode(MOVE_Flying);
	   return true;
   }

   return false;
}

// Called every frame
void UHangLedgeComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	ForwardTrace();
	HeightTrace();

	switch (HangLedgeState)
	{
	case EHangLedgeState::None:	
		ForwardDownTrace();
		BackwardDownTrace();
		if(CanForwardDrop)
		 GEngine->AddOnScreenDebugMessage(-1,0.0f,FColor::Red,"CanForwardDrop");
		if(CanBackDrop)
		 GEngine->AddOnScreenDebugMessage(-1,0.0f,FColor::Red,"CanBackDrop");
		break;
	case EHangLedgeState::ClimbingLedge:
		break;
	case EHangLedgeState::Hanging:
	    LeftTrace();
		RightTrace();
		break;
	case  EHangLedgeState::DropToLedge:
	   break;
	case EHangLedgeState::TurnToDrop:
	   TickTurnToLedge(DeltaTime);
	   break;
	default:
		break;
	}
	
}

void UHangLedgeComponent::HandleMoveInput(float AxisValue)
{
   if(HangLedgeState!=EHangLedgeState::Hanging)
      return;

   if (AxisValue < 0.f)
	   if (!CanMoveLeft)
		   AxisValue=0.f;
	     

   if(AxisValue>0.f)
	   if (!CanMoveRight)
		   AxisValue=0.f;

   FVector Direction = FVector::CrossProduct(JumpWallNormal, FVector::UpVector).GetSafeNormal();
   PlayerCharacter->AddMovementInput(Direction, AxisValue);


}

