// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/GrappleComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Grapple/GrapplePoint.h"
#include "Character/PlayerCharacter.h"
#include "Camera/CameraComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameFramework\CharacterMovementComponent.h"
#include "CableComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values for this component's properties
UGrappleComponent::UGrappleComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UGrappleComponent::BeginPlay()
{
	Super::BeginPlay();

	PlayerCharacter=GetOwner<APlayerCharacter>();
	
}


void UGrappleComponent::CheckforGrapplePoint()
{
	if (!UKismetSystemLibrary::SphereTraceMultiForObjects(this, PlayerCharacter->GetActorLocation(), PlayerCharacter->GetActorLocation(), DetectionRadius,
		ObjectTypes, false, TArray<AActor*>(), EDrawDebugTrace::None, OutHits, true))
	{
		DeactivateGrapplePoint();
		return;
	}
	
	HighestDotProduct=0.7f;
	
	for (const FHitResult& Hit : OutHits)
	{
	    float Dot= FVector::DotProduct(PlayerCharacter->GetFollowCamera()->GetForwardVector(),
			(Hit.GetActor()->GetActorLocation() - PlayerCharacter->GetFollowCamera()->GetComponentLocation()).GetSafeNormal());
		if (!(Dot> HighestDotProduct))
		{
			DeactivateGrapplePoint();
			continue;
		}

		DetectedActor=Hit.GetActor();
		HighestDotProduct=Dot;
	}

	ActivateGrapplePoint();
} 

void UGrappleComponent::DeactivateGrapplePoint()
{
    if(!GrapplePoint)
	  return;

	GrapplePoint->Deactivate();
	GrapplePoint=nullptr;
}

void UGrappleComponent::ActivateGrapplePoint()
{
   if(!DetectedActor)
     return;

    FHitResult OutHit;
    UKismetSystemLibrary::LineTraceSingle(this,PlayerCharacter->GetFollowCamera()->GetComponentLocation(),DetectedActor->GetActorLocation(),
    UEngineTypes::ConvertToTraceType(ECC_Visibility),false,TArray<AActor*>(),EDrawDebugTrace::None,OutHit,false);

    if(OutHit.GetActor()==DetectedActor)
	{
		if (DetectedActor != GrapplePoint)
		{
		   AGrapplePoint* TempPoint=Cast<AGrapplePoint>(DetectedActor);
		   DeactivateGrapplePoint();
		   GrapplePoint=TempPoint;
		   GrapplePoint->Activate(PlayerCharacter,this);
	    }
	}
	else
	{
	   DeactivateGrapplePoint();
	}
}

// Called every frame
void UGrappleComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	CheckforGrapplePoint();

	if(!bInGrapplingAnimation)
	  return;

	MoveRope();

	if(!bMovingWithGrapple)
	  return;

	GrapplingMovement();
}

void UGrappleComponent::Grapple()
{
   if(!GrapplePoint)
      return;
   
   if(!((PlayerCharacter->GetActorLocation()-GrapplePoint->GetActorLocation()).Size()<= GrappleThrowDistance
       &&CurrentGrapplePoint!=GrapplePoint))
	   return;

   if (bMovingWithGrapple)
   {
     PlayerCharacter->LaunchCharacter((GrapplingDestination-PlayerCharacter->GetActorLocation()).GetSafeNormal()*200.0f,false,false);
   }

   bInGrapplingAnimation=true;
   bMovingWithGrapple=false;
   CurrentGrapplePoint=GrapplePoint;
   GrapplingDestination=CurrentGrapplePoint->GetLandingZoneLocation();

   FRotator GrappleRotation=FRotator(0.f,UKismetMathLibrary::FindLookAtRotation(PlayerCharacter->GetActorLocation(),GrapplingDestination).Yaw,0.f);
   PlayerCharacter->SetActorRotation(GrappleRotation);

   RopeBaseLength=(PlayerCharacter->GetActorLocation()-GrapplingDestination).Size();
   CurrentGrapplePoint->Use();

   PlayerCharacter->SetCharacterState(ECharacterState::Grapple);

   if(PlayerCharacter->GetCharacterMovement()->IsFalling())
     PlayerCharacter->GetMesh()->GetAnimInstance()->Montage_Play(AirGrappleMontage);
   else
     PlayerCharacter->GetMesh()->GetAnimInstance()->Montage_Play(GroundGrappleMontage);
}

void UGrappleComponent::StartGrapplingMovement()
{
    UGameplayStatics::PlaySound2D(this,JumpSound);
	PlayerCharacter->GetCharacterMovement()->GravityScale=0.f;
	PlayerCharacter->GetCharacterMovement()->StopMovementImmediately();
	StartingPosition=PlayerCharacter->GetActorLocation();
	PlayerCharacter->GetCharacterMovement()->SetMovementMode(MOVE_Falling);
	bMovingWithGrapple=true;
}

void UGrappleComponent::MoveRope()
{
   UAnimInstance* AnimInstance= PlayerCharacter->GetMesh()->GetAnimInstance();

   UAnimMontage* CurrentMontage= AnimInstance->GetCurrentActiveMontage();
   if(!CurrentMontage)
      return;

   float MontagePosition=AnimInstance->Montage_GetPosition(CurrentMontage);

   bool bIsGround= CurrentMontage== GroundGrappleMontage;

   UCurveFloat* LengthCurve=bIsGround?GroundRopeLength:AirRopeLength;
   PlayerCharacter->GetRope()->CableLength=LengthCurve->GetFloatValue(MontagePosition)*RopeBaseLength;

   UCurveFloat* PositionCurve= bIsGround? GroundRopePosition : AirRopePosition;
   FVector Location=FMath::Lerp(PlayerCharacter->GetMesh()->GetSocketLocation(TEXT("hand_l")),CurrentGrapplePoint->GetActorLocation(),
                                PositionCurve->GetFloatValue(MontagePosition));

   PlayerCharacter->GetKunai()->SetWorldLocation(Location);
   
}

void UGrappleComponent::GrapplingMovement()
{
	UAnimInstance* AnimInstance = PlayerCharacter->GetMesh()->GetAnimInstance();

	UAnimMontage* CurrentMontage = AnimInstance->GetCurrentActiveMontage();
	if(!CurrentMontage)
	  return;

	float MontagePosition = AnimInstance->Montage_GetPosition(CurrentMontage);

	bool bIsGround = CurrentMontage == GroundGrappleMontage;

	UCurveFloat* SpeedCurve=bIsGround?GroundSpeedCurve:AirSpeedCurve;
	FVector CharacterPosition=FMath::Lerp(StartingPosition,GrapplingDestination,SpeedCurve->GetFloatValue(MontagePosition));

	UCurveFloat* HeightCurve=bIsGround?GroundHeightOffsetCurve:AirHeightOffsetCurve;
	FVector FinalPosition=CharacterPosition+FVector(0.f,0.f,HeightCurve->GetFloatValue(MontagePosition));

	PlayerCharacter->SetActorLocation(FinalPosition);
}

void UGrappleComponent::RopeVisibility(bool bVisible)
{
    PlayerCharacter->GetRope()->SetVisibility(bVisible);
	PlayerCharacter->GetKunai()->SetVisibility(bVisible);
}

void UGrappleComponent::ResetMovement()
{
   bMovingWithGrapple=false;
   CurrentGrapplePoint=nullptr;
   PlayerCharacter->GetCharacterMovement()->GravityScale=1.f;
   bInGrapplingAnimation=false;
   PlayerCharacter->GetCharacterMovement()->SetMovementMode(MOVE_Walking);
   PlayerCharacter->SetCharacterState(ECharacterState::Idle);
}

