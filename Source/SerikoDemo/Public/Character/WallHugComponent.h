// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "WallHugComponent.generated.h"

class APlayerCharacter;

UENUM(BlueprintType)
enum class EWallHugState :uint8
{
	None,
	MovingToWall,
	OnWall
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnWallHugEnd);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SERIKODEMO_API UWallHugComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UWallHugComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere,BlueprintReadWrite,Category=WallHug)
	APlayerCharacter* PlayerCharacter;

	UPROPERTY(VisibleAnywhere,BlueprintReadWrite,Category=WallHug)
	EWallHugState WallHugState;

	UPROPERTY(VisibleAnywhere,BlueprintReadWrite,Category=WallHug)
	FVector MoveToWallEnd;

	UPROPERTY(VisibleAnywhere,BlueprintReadWrite,Category=WallHug)
	FVector MoveToWallBegin;

	UPROPERTY(VisibleAnywhere,BlueprintReadWrite,Category=WallHug)
	FRotator MoveToWallRotBegin;

	UPROPERTY(VisibleAnywhere,BlueprintReadWrite,Category=WallHug)
	FRotator MoveToWallRotEnd;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = WallHug)
	float MoveToWallTime;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = WallHug)
	float MoveToWallDuration;

	FHitResult OnWallLineOutHit;

	FHitResult OnWallLineCapsuleOutHit;

	FVector WallNormal;

	bool FindCloestWall(FVector LineTraceStart,int32 NumLineTraces,float& _WallDist,FVector& _WallLocation,FVector& _WallNormal);

	bool LineTrace(FVector Start,float RotationAngleFromForward,FHitResult& OutHit);

	void TickMovingToWall(float DeltaSeconds);

	void TickOnWall(float DeltaSeconds);

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	bool TryBeginWallHug();

	void HandleMovementInput(float Scale,bool Right);

	FOnWallHugEnd OnWallHugEnd;

	UFUNCTION(BlueprintCallable)
	void EndWallHug();
};
