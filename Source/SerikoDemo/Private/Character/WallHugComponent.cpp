// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/WallHugComponent.h"
#include "Character/PlayerCharacter.h"
#include "Kismet/KismetMathLibrary.h"
#include "SerikoDemo/SerikoDemo.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

// Sets default values for this component's properties
UWallHugComponent::UWallHugComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	MoveToWallDuration=0.25f;
}


// Called when the game starts
void UWallHugComponent::BeginPlay()
{
	Super::BeginPlay();

	PlayerCharacter=GetOwner<APlayerCharacter>();
	
}


bool UWallHugComponent::FindCloestWall(FVector LineTraceStart, int32 NumLineTraces, float& _WallDist, FVector& _WallLocation, FVector& _WallNormal)
{
   float ClosestWallDist=0;
   FVector ClosestWallLocation=FVector::ZeroVector;
   FVector ClosestWallNormal=FVector::ZeroVector;

   float CurrentWallDist;
   FVector CurrentWallLocation;
   FVector CurrentWallNormal;

   FHitResult OutHit;
   for (int32 i = 0; i < NumLineTraces - 1; ++i)
   {
	   if (LineTrace(LineTraceStart, i * 360.0f / NumLineTraces, OutHit))
	   {
		   CurrentWallDist=OutHit.Distance;
		   CurrentWallLocation=OutHit.Location;
		   CurrentWallNormal=OutHit.Normal;

		   if (CurrentWallDist < ClosestWallDist || ClosestWallNormal == FVector::ZeroVector)
		   {
			   ClosestWallDist=CurrentWallDist;
			   ClosestWallLocation=CurrentWallLocation;
			   ClosestWallNormal=CurrentWallNormal;
		   }
	   }
   }

   _WallDist=ClosestWallDist;
   _WallLocation=ClosestWallLocation;
   _WallNormal=ClosestWallNormal;

   return ClosestWallNormal!=FVector::ZeroVector;

}

bool UWallHugComponent::LineTrace(FVector Start, float RotationAngleFromForward, FHitResult& OutHit)
{
   FVector End=Start+UKismetMathLibrary::RotateAngleAxis(PlayerCharacter->GetActorForwardVector(),RotationAngleFromForward,FVector(0.f,0.f,1.f))*100;
  
  return UKismetSystemLibrary::LineTraceSingle(this,Start,End,UEngineTypes::ConvertToTraceType(WALLTRACE),false,TArray<AActor*>(),EDrawDebugTrace::None,OutHit,true);

}

void UWallHugComponent::TickMovingToWall(float DeltaSeconds)
{
   MoveToWallTime=FMath::Clamp(DeltaSeconds/MoveToWallDuration+MoveToWallTime,0.0f,1.0f);
   PlayerCharacter->SetActorLocation(FMath::Lerp(MoveToWallBegin,MoveToWallEnd,MoveToWallTime),true);
   PlayerCharacter->SetActorRotation(FMath::Lerp(MoveToWallRotBegin,MoveToWallRotEnd,MoveToWallTime));

   if (MoveToWallTime >= 1.0f)
   {
	   WallHugState=EWallHugState::OnWall;
   }
} 

void UWallHugComponent::TickOnWall(float DeltaSeconds)
{
	if (LineTrace(PlayerCharacter->GetActorLocation(), 180.0f, OnWallLineOutHit))
	{
	   WallNormal=OnWallLineOutHit.Normal;
	   
	   UCapsuleComponent* CapsuleComponent=PlayerCharacter->GetCapsuleComponent();

	   if (
		   !UKismetSystemLibrary::CapsuleTraceSingle(this, CapsuleComponent->GetComponentLocation(),
			   CapsuleComponent->GetComponentLocation() - WallNormal,
			   CapsuleComponent->GetScaledCapsuleRadius(),
			   CapsuleComponent->GetScaledCapsuleHalfHeight(),
			   UEngineTypes::ConvertToTraceType(WALLTRACE),
			   false, TArray<AActor*>(), EDrawDebugTrace::None,
			   OnWallLineCapsuleOutHit, true)
		   )
	   {
		   PlayerCharacter->AddMovementInput(WallNormal,-1.f);
	   }
	   PlayerCharacter->SetActorRotation(
	      UKismetMathLibrary::RInterpTo(PlayerCharacter->GetActorRotation(),UKismetMathLibrary::MakeRotFromZX(FVector::UpVector,WallNormal),DeltaSeconds,10.0f)
	   );
    }
	else
	{
		EndWallHug();
	}
}

// Called every frame
void UWallHugComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	switch (WallHugState)
	{
	case EWallHugState::None:
		break;
	case EWallHugState::MovingToWall:
	    TickMovingToWall(DeltaTime);
		break;
	case EWallHugState::OnWall:
	    TickOnWall(DeltaTime);
		break;
	default:
		break;
	}
}

bool UWallHugComponent::TryBeginWallHug()
{
   if(WallHugState!=EWallHugState::None)
      return false;
    
	float WallDist;
	FVector WallLocation;
	FVector _WallNormal;
	if (FindCloestWall(PlayerCharacter->GetActorLocation(), 10.0f, WallDist, WallLocation, _WallNormal))
	{
		MoveToWallEnd=WallLocation;
		MoveToWallBegin=PlayerCharacter->GetActorLocation();
		MoveToWallRotBegin=PlayerCharacter->GetActorRotation();
		MoveToWallRotEnd=UKismetMathLibrary::MakeRotFromZX(FVector::UpVector,_WallNormal);
		MoveToWallTime=0.0f;
		WallHugState=EWallHugState::MovingToWall;
		PlayerCharacter->GetCharacterMovement()->bOrientRotationToMovement = false;
		PlayerCharacter->GetCharacterMovement()->MaxWalkSpeed = 100.0f;
		return true;
	}

	return false;
}

void UWallHugComponent::HandleMovementInput(float Scale, bool Right)
{
	if(WallHugState!=EWallHugState::OnWall)
	  return;

	if (WallNormal != FVector::ZeroVector)
	{
		if (Right)
		{
			FVector Direction=FVector::CrossProduct(WallNormal,FVector::UpVector).GetSafeNormal();
			PlayerCharacter->AddMovementInput(Direction,Scale);
		}
		else
		{
			if(Scale<-0.1f)
			   EndWallHug();
		}
	}
}

void UWallHugComponent::EndWallHug()
{
    WallHugState=EWallHugState::None;
	PlayerCharacter->GetCharacterMovement()->bOrientRotationToMovement=true;
	PlayerCharacter->GetCharacterMovement()->MaxWalkSpeed=600.0f;
	OnWallHugEnd.Broadcast();
}

