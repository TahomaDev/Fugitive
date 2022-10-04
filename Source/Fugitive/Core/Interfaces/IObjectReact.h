// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Fugitive/Types.h"
#include "UObject/Interface.h"
#include "IObjectReact.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UIObjectReact : public UInterface
{
	GENERATED_BODY()
public:
	
};

/**
 * 
 */
class FUGITIVE_API IIObjectReact
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Object React")	
    EObjectAction HowAreYou(AActor* Actor);
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Object React")	
    FItemInfo WhatIGet(AActor* Actor);
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Object React")	
    void SetAmount(AActor* Actor, uint8 Amount);
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Object React")	
	bool GetInteractTime(float &Time);


	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Object React")	
	void CustomDephForSelectEnable(bool bIsEnable);
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Object React")	
    bool HaveYouCustomDephForSelect();

};