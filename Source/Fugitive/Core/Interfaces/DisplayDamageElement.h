// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UObject/Interface.h"
#include "DisplayDamageElement.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UDisplayDamageElement : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class FUGITIVE_API IDisplayDamageElement
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Set Damage Value")	
    void SetDamageValue(float Damage, float CurrentHealth);
};
