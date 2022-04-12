// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "BeArmedInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UBeArmedInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class SERIKODEMO_API IBeArmedInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

    virtual void SetBeArmed()=0;

	virtual void CancelBeArmed()=0;

	virtual void ShowCanBeExecute() = 0;

	virtual void HideCanbeExecute() = 0;

	virtual bool CanShow() = 0;

	virtual bool CanHide() = 0;

	virtual bool IsShowing() = 0;
};
