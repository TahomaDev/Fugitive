// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "PickupItem.h"
#include "InteractionItem.generated.h"

/**
 * 
 */
UCLASS()
class FUGITIVE_API AInteractionItem : public APickupItem
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;

private:
	// Functions
	virtual void OnKill() override;

	void ItemRecoveryTimer();

	// Variables
	FTimerHandle ItemRecoveryTimerHandle;
	FItemInfo ItemInfo_Real;
	
	bool bIsInvisible = false;
	

public:
	/** @Tahoma. Возвращаем на запрос информацию, что этот объект можно собрать
	 * если он сейчас не в состоянии восстановления
	 */
	virtual EObjectAction HowAreYou_Implementation(AActor* Actor);
	virtual bool GetInteractTime_Implementation(float &Time);

	// Время сбора в секундах
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float InteractTime = 5;
	
	// Время восстановления в секундах
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float RecoveryTime = 60 * 15;
};
