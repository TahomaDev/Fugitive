// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "PickupItem.h"
#include "DropItem.generated.h"

/**
 * 
 */
UCLASS()
class FUGITIVE_API ADropItem : public APickupItem
{
	GENERATED_BODY()
protected:
	virtual void BeginPlay() override;

private:
	void SetLocationToGround();
	
public:
	UFUNCTION(BlueprintCallable)
	void Init(FItemInfo NewItemInfo);
	
};
