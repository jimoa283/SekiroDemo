// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/BasePlayerCharacter.h"
#include "PlayerCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class APlayerLikEnemyCharacter;
class UWidgetComponent;
class UDangerWidget;
class UBoxComponent;
class UShowBackEnemyWidget;
class UPlayerWidget;
class URPGTokenItem;
class UGrappleComponent;
class UCableComponent;
class UWallHugComponent;
class UHangLedgeComponent;
class UInputBufferComponent;
class UPlayerHUD;


USTRUCT(BlueprintType)
struct FExeucteInfo
{
	GENERATED_BODY()

public:
    
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category=Execute)
	float TargetDistance;

	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category=Execute)
	FGameplayTagContainer ExecuteTag;

	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category=Execute)
	UAnimMontage* TargetMontage;

};

USTRUCT(BlueprintType)
struct FShowBackEnemyInfo
{
	GENERATED_BODY()

public:
    
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category=ShowBackEnemy)
	bool IsLeft;

	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category=ShowBackEnemy)
	float Angle;

	FShowBackEnemyInfo(){}

	FShowBackEnemyInfo(bool _IsLeft,float _Angle)
	{
		IsLeft=_IsLeft;
		Angle=_Angle;
	}

};

/**
 * 
 */
UCLASS()
class SERIKODEMO_API APlayerCharacter : public ABasePlayerCharacter
{
	GENERATED_BODY()
	
public:
        
	APlayerCharacter();

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:

	virtual void BeginPlay() override;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Component)
	USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category=Component)
	UCameraComponent* FollowCamera;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category=Component)
	UBoxComponent* ExecuteCheckRange;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category=Component)
	UBoxComponent* JumpTwiceCheckRange;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category=Component)
	UWidgetComponent* ShowBackEnemyWidget;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category=Component)
	UGrappleComponent* GrappleComponent;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category=Component)
	UWallHugComponent* WallHugComponent;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category=Component)
	UHangLedgeComponent* HangLedgeComponent;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category=Component)
	UInventoryComponent* Inventory;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category=Component)
	UCableComponent* Rope;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category=Component)
	UStaticMeshComponent* Kunai;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category=Component)
	UArrowComponent* LeftHangArrow;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category=Component)
	UArrowComponent* RightHangArrow;

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category=Inventory)
	TSubclassOf<UUserWidget> WBPInventory;

	UPROPERTY(VisibleAnywhere,BlueprintReadWrite,Category=Inventory)
	UUserWidget* InventoryWidget;

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category=Abilities)
	FGameplayTagContainer JumpTag;

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category=Abilities)
	FGameplayTag EndJumpTag;

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category=Abilities)
	FGameplayTag JumpTwiceTag;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Inventory)
	TMap<FPrimaryAssetType, FGameplayAbilitySpecHandle> ItemAbilityMap;

	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category=Arm)
	float UpdateArmTime;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Arm)
	bool IsArmed;

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category=Arm)
	float TryToArmRange;

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category=Arm)
	float TryToArmDistance;

	TArray<ABaseCharacter*> TryToArmTagets;

	FTimerHandle UpdateArmHandle;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category=Danger)
	UWidgetComponent* DangerWidget;

	UPROPERTY()
	UDangerWidget* WBPDanger;

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category=HUD)
	TSubclassOf<UPlayerHUD> WBPPlayerHUD;

	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category=JumpTwice)
	float JumpTwiceCheckDistance;

	UPROPERTY(BlueprintReadWrite,Category=JumpTwice)
	bool HasJumpTwice;

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category=Execute)
	FExeucteInfo ForwardExecuteInfo;

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category=Execute)
	FExeucteInfo BackExecuteInfo;

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category=Execute)
	float ExecuteWaitTime;

	FTimerHandle DoExecuteHandle;

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category=ShowBackEnemy)
	TMap<ABaseCharacter*,FShowBackEnemyInfo> ShowBackEnemyMap;

	UPROPERTY(VisibleAnywhere,BlueprintReadWrite,Category=ShowBackEnemy)
	UShowBackEnemyWidget* WBPShowBackEnemy;

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category=Dead)
	UAnimMontage* DeadMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = TargetLoseBlance)
	float DefaultLength;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = TargetLoseBlance)
	FVector DefaultOffset;

	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category=TargetLoseBlance)
	float TargetLoseBlanceLength;

	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category=TargetLoseBlance)
	FVector TargetLoseBlanceOffset;

	float TargetLength;

	FVector TargetOffset;

	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category=TargetLoseBlance)
	float UpdateSpeed;

	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category=TargetLoseBlance)
	float TargetLoseBlanceUpdateSpeed;

	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category=TargetLoseBlance)
	float ReturnDefaultUpdateSpeed;

	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category=TargetLoseBlance)
	float TimeDownValue;

	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category=TargetLoseBlance)
	float TimeDownTime;	

	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category=TargetLoseBlance)
	float TimeDownWaitTime;

	FTimerHandle UpdateCameraPositionHandle;

	FTimerHandle TimeDownHandle;

	FTimerHandle TimeDownWaitHandle;

	bool CanMoveForward;

	bool CanMoveBack;

	bool CanMoveRight;

	bool CanMoveLeft;

	void LookUp(float AxisValue);

	void LookRight(float AxisValue);

	void DoMoveForward(float AxisValue);

	void DoMoveRight(float AxisValue);

	void DoWallHugMove(float Scale,bool Right);

	UFUNCTION(BlueprintCallable)
	void DoAttack();

	void DoCrouch(); 

	void DoJump();

	void DoArm();

	void TryToArm();

	void CancelArm();

	void ResetView();

	bool DoExecute();

	UFUNCTION()
	void DoSpecificExecute();

	void DoGrapple();

	void DoWallHug();

	void DoClimbLedge();

	void DoCancelHang();

	void DoDropToLedge();

	void UseItem();

	UFUNCTION(BlueprintCallable)
	void UseSkill();

	void UseArtificialHand();

	void ChangeToUpUseItem();

	void ChangeToDownUseItem();

	void ChangeAritficialHand();

	void ToggleInventoryWidget();

	UFUNCTION()
	void JumpTwiceOnTarget(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void HandleLanded(const FHitResult& Hit);

	UFUNCTION()
	void UpdateShowBackEnemy();

	UFUNCTION()
	void OnWallHugEnd();

	UFUNCTION()
	void UpdateTargetLoseBlanceCamera();

	UFUNCTION()
	void StartTargetLoseBlanceTimeDown();

	UFUNCTION()
	void EndTargetLoseBlanceTimeDown();

	UFUNCTION(BlueprintCallable,Category=Abilities)
	bool ActivateAbilitiesWithItem(const FGameplayAbilitySpecHandle& ItemAbility, bool bAllowRemoteActivation=true);

	virtual void ToDead() override;

public:

	virtual void Tick(float DeltaTime) override;
    
	UFUNCTION(BlueprintPure,BlueprintCallable)
	FName GetDirection();

	UFUNCTION(BlueprintCallable)
	void ShowDanger();

	UFUNCTION()
	void HideDanger();

	UFUNCTION(BlueprintCallable)
	void AddNewShowBackEnemy(ABaseCharacter* Target);

	UFUNCTION(BlueprintCallable)
	void RemoveShowBackEnemy(ABaseCharacter* Target);

	UFUNCTION(BlueprintCallable)
	void AddMoney(int32 DeltaValue);

	UFUNCTION(BlueprintCallable)
	FORCEINLINE UCameraComponent* GetFollowCamera() const {return FollowCamera;}

	UFUNCTION(BlueprintCallable)
	UCableComponent* GetRope() const;

	UFUNCTION()
	void StartCommand(FString Command);

	UFUNCTION(BlueprintCallable)
	FORCEINLINE UStaticMeshComponent* GetKunai() const {return Kunai;}

	FVector GetLeftHangArrowLocation() const;

	FVector GetRightHangArrowLocation() const;

	UFUNCTION(BlueprintCallable)
	void ChangeEquipItem(URPGItem* ChangeItem,FPrimaryAssetType ChangeItemType);

	UFUNCTION(BlueprintCallable)
	void TargetLoseBlanceHitPause(ABaseCharacter* Target);

	UFUNCTION(BlueprintCallable)
	void ReturnDefaultCameraPosition();
};
