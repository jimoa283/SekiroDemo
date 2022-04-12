// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HangLedgeComponent.generated.h"

class APlayerCharacter;

UENUM(BlueprintType)
enum class EHangLedgeState:uint8
{
    None,
	ClimbingLedge,
	Hanging,
	DropToLedge,
	TurnToDrop
};


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SERIKODEMO_API UHangLedgeComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UHangLedgeComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category=BaseHangLedge)
	APlayerCharacter* PlayerCharacter;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = BaseHangLedge)
	EHangLedgeState HangLedgeState;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = MoveHangLedge)
	float HangMoveSpeed;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = DropHangLedge)
	float DropTraceHeight;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = DropHangLedge)
	float DropTraceStartZ;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = DropHangLedge)
	float DistanceToWall;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = DropHangLedge)
	float MoveToLedgeDuration;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = DropHangLedge)
	float FindWallTraceStartZ;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = DropHangLedge)
	int32 NumLineTraces;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category =DropHangLedge)
	UAnimMontage* DropToLedgeMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = ClimbHangLedge)
	UAnimMontage* ClimbLedgeMontage;

	FVector JumpHeightLocation;

	FVector JumpWallLocation;

	FVector JumpWallNormal;

	FVector DropWallNormal;

	FVector MoveToLedgeBegin;

	FVector MoveToLedgeEnd;

	FRotator MoveToLedgeRotBegin;

	FRotator MoveToLedgeRotEnd;

	float MoveToLedgeTime;

	bool CanMoveLeft;

	bool CanMoveRight;

	bool CanBackDrop;

	bool CanForwardDrop;

	FHitResult ForwardTraceOutHit;

	FHitResult HeightTraceOutHit;

	FHitResult LeftTraceOutHit;

	FHitResult RightTraceOutHit;

	FHitResult ForwardDownTraceOutHit;

	FHitResult BackDownTraceOutHit;

	void ForwardTrace();

	void HeightTrace();

	void LeftTrace();

	void RightTrace();

	void GrabLedge();

	void ForwardDownTrace();

	void BackwardDownTrace();

	bool FindCloestWall(FVector LineTraceStart, int32 _NumLineTraces, float& _WallDist, FVector& _WallLocation, FVector& _WallNormal);

	bool LineTrace(FVector Start, float RotationAngleFromForward, FHitResult& OutHit);

	void StartDropToLedge();

	UFUNCTION(BlueprintCallable)
	void EndDropToLedge();

	UFUNCTION(BlueprintCallable)
	void OnCompleteHang();

	UFUNCTION(BlueprintCallable)
	void EndClimbLedge();

	void TickTurnToLedge(float DeltaSeconds);

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void HandleMoveInput(float AxisValue);

	void DoClimbLedge();

	void CancelHang();

	bool TryToDropToLedge();
};
