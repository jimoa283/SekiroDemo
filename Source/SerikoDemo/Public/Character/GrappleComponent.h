// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GrappleComponent.generated.h"

class AGrapplePoint;
class APlayerCharacter;
class UCurveFloat;
class UCableComponent;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SERIKODEMO_API UGrappleComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UGrappleComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	APlayerCharacter* PlayerCharacter;

	UPROPERTY(VisibleAnywhere,BlueprintReadWrite,Category=GrappleCheck)
	AActor* DetectedActor;

	UPROPERTY(VisibleAnywhere,BlueprintReadWrite,Category=GrappleCheck)
	AGrapplePoint* GrapplePoint;

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category=GrappleCheck)
	float GrappleThrowDistance;

	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category=GrappleCheck)
	float DetectionRadius;

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category=GrappleCheck)
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category=GrappleCheck)
	float HighestDotProduct;

	TArray<FHitResult> OutHits; 

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category=GrapplingMovement)
	bool bMovingWithGrapple;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category=GrapplingMovement)
	FVector GrapplingDestination;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category=GrapplingMovement)
	AGrapplePoint* CurrentGrapplePoint;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = GrapplingMovement)
	bool bInGrapplingAnimation;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category=GrapplingMovement)
	FVector StartingPosition;

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category=GrapplingMovement)
	UCurveFloat* GroundHeightOffsetCurve;

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category=GrapplingMovement)
	UCurveFloat* GroundSpeedCurve;

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category=GrapplingMovement)
	UCurveFloat* AirSpeedCurve;

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category=GrapplingMovement)
	UCurveFloat* AirHeightOffsetCurve;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = GrapplingMovement)
	UAnimMontage* AirGrappleMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = GrapplingMovement)
	UAnimMontage* GroundGrappleMontage;

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category=GrapplingMovement)
	USoundBase* JumpSound;

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category=RopeControl)
	float RopeBaseLength;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = RopeControl)
	UCurveFloat* GroundRopeLength;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = RopeControl)
	UCurveFloat* GroundRopePosition;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = RopeControl)
	UCurveFloat* AirRopeLength;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = RopeControl)
	UCurveFloat* AirRopePosition;

	void CheckforGrapplePoint();

	void DeactivateGrapplePoint();

	void ActivateGrapplePoint();

	void MoveRope();

	void GrapplingMovement();

	UFUNCTION(BlueprintCallable)
	void RopeVisibility(bool bVisible);

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;	

	void Grapple();

	UFUNCTION(BlueprintCallable)
	void StartGrapplingMovement();

	UFUNCTION(BlueprintCallable)
	void ResetMovement();

	UFUNCTION(BlueprintCallable)
	FORCEINLINE float GetGrappleThrowDistance() const { return GrappleThrowDistance; }

	UFUNCTION(BlueprintCallable)
	FORCEINLINE float GetDetectionRadius() const {return DetectionRadius;}

	UFUNCTION(BlueprintCallable)
	FORCEINLINE bool GetIsMovingWithGrapple() const {return bMovingWithGrapple;}
   
};
