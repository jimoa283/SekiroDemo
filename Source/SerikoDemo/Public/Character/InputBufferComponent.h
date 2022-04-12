// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/InputType.h"
#include "Containers/CircularBuffer.h"
#include "Components/ActorComponent.h"
#include "InputBufferComponent.generated.h"

class APlayerCharacter;


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SERIKODEMO_API UInputBufferComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UInputBufferComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	int CurTick;
	bool CapturedInputThisFrame;

	APlayerCharacter* PlayerCharacter;

	// The map of inputs to InputTypes for access when determining if the player controlling this character has used a recognized input.
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Input")
	TMap<FKey,EInputType> InputToInputTypeMap;

	/* The array of inputs the player controlling this character has performes */
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Input")
	TArray<FInputInfo> InputBuffer;

	//The array of inputs the player controlling this character has performed (circular buffer).
	TCircularBuffer<FInputInfo> CircularInputBuffer=TCircularBuffer<FInputInfo>(60);

	/* The array of inputs with their corresponding charge times for charge Input */
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Input")
	TArray<FChargeInput> ChargeTimes;

	/* Commands that can be used a correct series of inputs has been pressed */
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Input")
	TArray<FCommand> CharacterCommands;

	/* Commands that have had their inputs pressed correctly and can be activated on the next update (Tick) or when other animations/states have completed */
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Input")
	TArray<FCommand> MoveBuffer;

	UFUNCTION(BlueprintCallable)
	void AddToInputMap(FKey _InputKey,EInputType _InputType);

	UFUNCTION(BlueprintCallable)
	void DetermineInputDataForBuffer(FKey _Key,EInputStatus _InputStatus);

	UFUNCTION(BlueprintCallable)
	void DetermineInputDataForBufferWhenPress(FKey _Key);

	UFUNCTION(BlueprintCallable)
	void DetermineInputDataForBufferWhenRelease(FKey _Key);

	/* Adds inputs to the input buffer */
	UFUNCTION(BlueprintCallable)
	void AddToInputToInputBuffer(FInputInfo _InputInfo);

	/* Check if the input buffer contains any sequences from the list of commands using hard-coded values */
	UFUNCTION(BlueprintCallable)
	void CheckInputBufferForCommand();

	UFUNCTION(BlueprintCallable)
	void CheckInputBufferForCommandUsingType();

	/* Determine which commands should be used based off of specific criteria (such as move length,difficulty,or relevance) */
	UFUNCTION(BlueprintCallable)
	void DetermineCommandToUse();

	/* Make the character begin using a command based off of the command's name */
	UFUNCTION(BlueprintCallable)
	void StartCommand(FString _CommandName);

	/* Begin tracking the time (number of frames) an input has been held */
	UFUNCTION(BlueprintCallable)
	void StartTrackingChargeTime(FChargeInput _InputToTrack);

	/* Reset the charge time for a pressed or held input (most often due to relating the input */
	UFUNCTION(BlueprintCallable)
	void ResetChargeTime(FChargeInput _InputToReset);

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
