// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Fugitive/Types.h"
#include "Components/ActorComponent.h"
#include "T_MinimapObject.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class FUGITIVE_API UT_MinimapObject : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UT_MinimapObject();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	UFUNCTION()
	void AddToRadar();
	UFUNCTION(BlueprintCallable)
	void RemoveFromRadar();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bDrawToMinimap = true;


	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EMinimapObjectType MinimapObjectType = EMinimapObjectType::Pickup;
};
