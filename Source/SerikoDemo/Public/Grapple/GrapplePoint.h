// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GrapplePoint.generated.h"

class UWidgetComponent;
class UBoxComponent;
class APlayerCharacter;
class UDetectionNode;
class UGrappleComponent;

UCLASS()
class SERIKODEMO_API AGrapplePoint : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGrapplePoint();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category=Component)
	UStaticMeshComponent* Detection;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category=Component)
	UWidgetComponent* Widget;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category=Component)
	UStaticMeshComponent* LandingZone;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category=Component)
	UBoxComponent* DeactivateZone;

	UPROPERTY(VisibleAnywhere,BlueprintReadWrite,Category=Grapple)
	bool bActive;

	UPROPERTY(VisibleAnywhere,BlueprintReadWrite,Category=Grapple)
	APlayerCharacter* PlayerCharacter;

	UPROPERTY(VisibleAnywhere,BlueprintReadWrite,Category=Grapple)
	UGrappleComponent* GrappleComponent;

	UPROPERTY(VisibleAnywhere,BlueprintReadWrite,Category=Grapple)
	UDetectionNode* DetectionNode;

	UPROPERTY(VisibleAnywhere,BlueprintReadWrite,Category=Grapple)
	bool bUsed;

	FTimerHandle UseHandle;

	UFUNCTION(BlueprintCallable)
	void CheckDistanceFromPlayer();

	UFUNCTION()
	void HandleDeativateZoneBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void HandleDeativateZoneEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	void Activate(APlayerCharacter* _PlayerCharacter,UGrappleComponent* _GrappleComponent);

	UFUNCTION(BlueprintCallable)
	void Deactivate();

	UFUNCTION(BlueprintCallable)
	void Use();

	UFUNCTION(BlueprintCallable)
	void Reactivate();

	UFUNCTION(BlueprintCallable)
	FORCEINLINE FVector GetLandingZoneLocation() {return LandingZone->GetComponentLocation()+FVector(0.f,0.f,110.0f);}



};
