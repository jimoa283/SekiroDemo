// Fill out your copyright notice in the Description page of Project Settings.


#include "Grapple/GrapplePoint.h"
#include "Components/BoxComponent.h"
#include "Components/WidgetComponent.h"
#include "UI/DetectionNode.h"
#include "Character/PlayerCharacter.h"
#include "Kismet/KismetMathLibrary.h"
#include "Character/GrappleComponent.h"

// Sets default values
AGrapplePoint::AGrapplePoint()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Detection=CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Detection"));
	RootComponent=Detection;

	Widget=CreateDefaultSubobject<UWidgetComponent>(TEXT("Widget"));
	Widget->SetupAttachment(Detection);

	LandingZone=CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LandingZone"));
	LandingZone->SetupAttachment(Detection);

	DeactivateZone=CreateDefaultSubobject<UBoxComponent>(TEXT("DeactivateZone"));
	DeactivateZone->SetupAttachment(Detection);
}

// Called when the game starts or when spawned
void AGrapplePoint::BeginPlay()
{
	Super::BeginPlay();

	DetectionNode=Cast<UDetectionNode>(Widget->GetWidget());
	DetectionNode->SetVisibility(ESlateVisibility::Hidden);
	
	DeactivateZone->OnComponentBeginOverlap.AddDynamic(this,&AGrapplePoint::HandleDeativateZoneBeginOverlap);
	DeactivateZone->OnComponentEndOverlap.AddDynamic(this,&AGrapplePoint::HandleDeativateZoneEndOverlap);
}

// Called every frame
void AGrapplePoint::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bActive && !bUsed)
	{
	   CheckDistanceFromPlayer();
	}
}

void AGrapplePoint::CheckDistanceFromPlayer()
{
   float Size=UKismetMathLibrary::MapRangeClamped((PlayerCharacter->GetActorLocation()-GetActorLocation()).Size(),
                                                    GrappleComponent->GetGrappleThrowDistance(),
													GrappleComponent->GetDetectionRadius(),
													80.0f,10.0f);

   DetectionNode->UpdateDistanceCheckImage(Size);
}

void AGrapplePoint::HandleDeativateZoneBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor==PlayerCharacter)
		Detection->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AGrapplePoint::HandleDeativateZoneEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
   if(OtherActor==PlayerCharacter)
      Detection->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

void AGrapplePoint::Activate(APlayerCharacter* _PlayerCharacter, UGrappleComponent* _GrappleComponent)
{
   if(bUsed)
     return;
   
   PlayerCharacter=_PlayerCharacter;
   GrappleComponent=_GrappleComponent;
   bActive=true;
   DetectionNode->SetVisibility(ESlateVisibility::Visible);
}

void AGrapplePoint::Deactivate()
{
   if(bUsed)
    return;
   
   bActive=false;
   
   DetectionNode->Deativate();
   
}

void AGrapplePoint::Use()
{
   bUsed=true;
   DetectionNode->PlayNodeUseAnimation();
   GetWorldTimerManager().SetTimer(UseHandle,this,&AGrapplePoint::Reactivate,2.0f,false);
}

void AGrapplePoint::Reactivate()
{
   bUsed=false;
}

