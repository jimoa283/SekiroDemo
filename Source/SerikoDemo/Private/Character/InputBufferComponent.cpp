// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/InputBufferComponent.h"
#include "Character/PlayerCharacter.h"

// Sets default values for this component's properties
UInputBufferComponent::UInputBufferComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UInputBufferComponent::BeginPlay()
{
	Super::BeginPlay();

	PlayerCharacter=GetOwner<APlayerCharacter>();
	
}


void UInputBufferComponent::AddToInputMap(FKey _InputKey, EInputType _InputType)
{
    
}


void UInputBufferComponent::DetermineInputDataForBuffer(FKey _Key, EInputStatus _InputStatus)
{
    int64 ChargeFrameCount=0;
	const EInputType& _InputType=InputToInputTypeMap[_Key];

	for (const FChargeInput& CurrentChargeInput : ChargeTimes)
	{
		if (CurrentChargeInput.InputType == _InputType)
		{
			ChargeFrameCount=CurrentChargeInput.ChargeFrames;
			break;
		}
	}

	FInputInfo _InputInfo;
	_InputInfo.ChargedFrames=ChargeFrameCount;
	_InputInfo.Frame= GFrameCounter;
	_InputInfo.InputStatus=_InputStatus;
	_InputInfo.InputType=_InputType;

	AddToInputToInputBuffer(_InputInfo);
}

void UInputBufferComponent::DetermineInputDataForBufferWhenPress(FKey _Key)
{
	if (!InputToInputTypeMap.Contains(_Key))
		return;
   
   DetermineInputDataForBuffer(_Key,EInputStatus::E_Press);
   FChargeInput _InputToTrack;
   _InputToTrack.ChargeFrames=0;
   _InputToTrack.InputType=InputToInputTypeMap[_Key];
   _InputToTrack.IsHoldingInput=false;
   StartTrackingChargeTime(_InputToTrack);
}

void UInputBufferComponent::DetermineInputDataForBufferWhenRelease(FKey _Key)
{
   if(!InputToInputTypeMap.Contains(_Key))
     return;
   
   DetermineInputDataForBuffer(_Key,EInputStatus::E_Release);
   FChargeInput _InputToTrack;
   _InputToTrack.ChargeFrames = 0;
   _InputToTrack.InputType = InputToInputTypeMap[_Key];
   _InputToTrack.IsHoldingInput = false;
   ResetChargeTime(_InputToTrack);
}

void UInputBufferComponent::AddToInputToInputBuffer(FInputInfo _InputInfo)
{

	if (!CapturedInputThisFrame)
	{
	  CircularInputBuffer[CurTick]=_InputInfo;
	  CapturedInputThisFrame=true;
    }
	else
	{
		// Multiple inputs were performed on the same frame;
	}

	CheckInputBufferForCommandUsingType();
}

void UInputBufferComponent::CheckInputBufferForCommand()
{
   
}

void UInputBufferComponent::CheckInputBufferForCommandUsingType()
{
	int CorrectSequenceCounter = 0;

	for (const FCommand& CurrentCommand : CharacterCommands)
	{
		CorrectSequenceCounter = CurrentCommand.InputTypes.Num() - 1;

		//Check if the player has used correct inputs to perform a command.
		for (int frame = 0; frame < CurrentCommand.MaxFramesBetweenInputs; ++frame)
		{
			int FrameToCheck = (CurTick - frame + CircularInputBuffer.Capacity()) % CircularInputBuffer.Capacity();
			const EInputType& Type = CircularInputBuffer[FrameToCheck].InputType;
			const EInputStatus& Status = CircularInputBuffer[FrameToCheck].InputStatus;
			int64 ChargedFrames = CircularInputBuffer[FrameToCheck].ChargedFrames;

			if (Type == CurrentCommand.InputTypes[CorrectSequenceCounter].InputType
				&& Status == CurrentCommand.InputTypes[CorrectSequenceCounter].InputStatus
				&& ChargedFrames >= CurrentCommand.InputTypes[CorrectSequenceCounter].RequiredChargeFrames)
			{
				--CorrectSequenceCounter;
			}
			else if (Type != EInputType::E_None)
			{
				CorrectSequenceCounter = CurrentCommand.InputTypes.Num() - 1;
			}

			if (CorrectSequenceCounter == -1)
			{
				MoveBuffer.Add(CurrentCommand);
				break;
			}
		}
	}
}

void UInputBufferComponent::DetermineCommandToUse()
{
	if (MoveBuffer.Num() > 0)
	{
		FCommand MoveToUse=MoveBuffer[0];
		for (int32 i = 1; i < MoveBuffer.Num(); ++i)
		{
			if (MoveToUse.InputTypes.Num() < MoveBuffer[i].InputTypes.Num())
			{
				MoveToUse=MoveBuffer[i];				
			}			
		}

		for (int32 CurrentCommand = 0; CurrentCommand < CharacterCommands.Num(); ++CurrentCommand)
		{
			if (MoveToUse.Name.Compare(CharacterCommands[CurrentCommand].Name) == 0)
			{
				CharacterCommands[CurrentCommand].HasUsedCommand = true;
				break;
			}
		}

		PlayerCharacter->StartCommand(MoveToUse.Name);
		MoveBuffer.Empty();
	}
}

void UInputBufferComponent::StartCommand(FString _CommandName)
{
	for (int32 CurrentCommand = 0; CurrentCommand < CharacterCommands.Num(); ++CurrentCommand)
	{
		if (_CommandName.Compare(CharacterCommands[CurrentCommand].Name) == 0)
		{
			CharacterCommands[CurrentCommand].HasUsedCommand=true;
		}
	}
}

void UInputBufferComponent::StartTrackingChargeTime(FChargeInput _InputToTrack)
{
	for (int i = 0; i < ChargeTimes.Num(); ++i)
	{
		if (ChargeTimes[i].InputType == _InputToTrack.InputType)
		{
			ChargeTimes[i].IsHoldingInput=true;
	   }
    }
}

void UInputBufferComponent::ResetChargeTime(FChargeInput _InputToReset)
{
	for (int i = 0; i < ChargeTimes.Num(); ++i)
	{
		if (ChargeTimes[i].InputType == _InputToReset.InputType)
		{
		    ChargeTimes[i].IsHoldingInput=false;
			ChargeTimes[i].ChargeFrames=0;
 	    }
    }
}

// Called every frame
void UInputBufferComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!CapturedInputThisFrame)
	{
		FInputInfo NoneInput;
		NoneInput.InputType=EInputType::E_None;
		NoneInput.InputStatus=EInputStatus::E_NONE;
		NoneInput.Frame=GFrameCounter;

		CircularInputBuffer[CurTick]=NoneInput;
	}
	else
	{
		CapturedInputThisFrame=false;
	}

	//Update all input charging values;
	for (int i = 0; i < ChargeTimes.Num(); ++i)
	{
		if (ChargeTimes[i].IsHoldingInput)
		{
			++ChargeTimes[i].ChargeFrames;
		}
	}

	if (CurTick < 59)
	{
		++CurTick;
	}
	else
	{
		CurTick=0;
	}

	// Reduce the amount of frames the stun still needs to occur to complete
	

	DetermineCommandToUse();
}


