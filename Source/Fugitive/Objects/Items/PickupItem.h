// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/PostProcessComponent.h"
#include "GameFramework/Actor.h"
#include "Fugitive/Types.h"
#include "Fugitive/Core/Interfaces/IObjectReact.h"
#include "PickupItem.generated.h"

class USphereComponent;
class UMyGameInstance;


UCLASS()
class FUGITIVE_API APickupItem : public AActor, public IIObjectReact
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APickupItem();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;	
	virtual void AmountRandom();
	virtual void OnKill();
	void VisualMeshInit();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pickup Item")
	USceneComponent* SceneComponent = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pickup Item")
	USphereComponent* CollisionSphere = nullptr;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Pickup Item")
	class USkeletalMeshComponent* ItemSkeletalMesh = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pickup Item")
	UStaticMeshComponent* ItemStaticMesh = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pickup Item")
	UPostProcessComponent* PostProcess = nullptr;

	UMyGameInstance* GameInstance;

	void CheckValidMeshes();

	// Flags
	bool bIsDropItem = false;

private:	  
	UMaterialInstance* ExcretionMaterial;
	

public:
	// Interface
	virtual void CustomDephForSelectEnable_Implementation(bool bIsEnable);
	virtual bool HaveYouCustomDephForSelect_Implementation() override;
	
	virtual EObjectAction HowAreYou_Implementation(AActor* Actor);
	virtual FItemInfo WhatIGet_Implementation(AActor* Actor);
	virtual void SetAmount_Implementation(AActor* Actor, uint8 Amount);
	
	
	UFUNCTION(BlueprintCallable)
	void SetCount(uint8 Count);
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Item")
	FItemInfo ItemInfo;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Item")
	bool bRandom = false;

	// Установка в true означает, что один из мешей указан
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Config")
	bool bIsBPItem = false;

};
