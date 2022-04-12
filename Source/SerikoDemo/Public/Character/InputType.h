#pragma once
#include "CoreMinimal.h"
#include "InputType.generated.h"

UENUM(BlueprintType)
enum class EInputType:uint8
{
   E_None                         UMETA(DisplayName="NONE"),
   E_Forward                      UMETA(DisplayName="FORWARD"),
   E_Back                         UMETA(DisplayName="BACK"),
   E_Right                        UMETA(DisplayName="RIGHT"),
   E_Left                         UMETA(DisplayName="LEFT"),
   E_Crouch                       UMETA(DisplayName="CROUCH"),
   E_Jump                         UMETA(DisplayName="JUMP"),
   E_Melee                        UMETA(DisplayName="MELEE"),
   E_Avoid                        UMETA(DisplayName="AVOID"),
   E_Defense                      UMETA(DisplayName="DEFENSE"),
   E_Grapple                      UMETA(DisplayName="GRAPPLE"),
   E_Action                       UMETA(DisplayName="ACTION"),
   E_SetWalkState                 UMETA(DisplayName="SETWALKSTATE"),
   E_ARM                          UMETA(DisplayName="ARM")
};

UENUM(BlueprintType)
enum class EInputStatus :uint8
{
	E_NONE                        UMETA(DisplayName="NONE"),
	E_Press                       UMETA(DisplayName="PRESS"),
	E_Release                     UMETA(DisplayName="RELEASE"),
	E_Hold                        UMETA(DisplayName="HOLD")
};

USTRUCT(BlueprintType)
struct FCommandInput
{
	GENERATED_BODY()

public: 
    
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Input")
	EInputType InputType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	EInputStatus InputStatus;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	int64 RequiredChargeFrames;

};

USTRUCT(BlueprintType)
struct FCommand
{
	GENERATED_BODY()

public:
     
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Input")
	FString Name;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Input")
	TArray<FCommandInput> InputTypes;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Input")
	int64 MaxFramesBetweenInputs=12;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Input")
	bool HasUsedCommand;
};

USTRUCT(BlueprintType)
struct FInputInfo
{
	GENERATED_BODY()

public: 
    
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
    EInputType InputType;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Input")
	EInputStatus InputStatus;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	int64 Frame;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	int64 ChargedFrames;
};

USTRUCT(BlueprintType)
struct FChargeInput
{
	GENERATED_BODY()

public:
    
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Input")
	EInputType InputType;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Input")
	int64 ChargeFrames;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	bool IsHoldingInput=false;
};


