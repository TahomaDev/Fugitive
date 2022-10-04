// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/ActorComponent.h"
#include "Fugitive/Types.h"
#include "T_InventoryComponent.generated.h"

class AFugitiveCharacter;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnUpdateInventorySlots);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnUpdateCharacterSlot, uint8, SlotIndex);

UCLASS(Blueprintable, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class FUGITIVE_API UT_InventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UT_InventoryComponent();

	UPROPERTY(BlueprintAssignable, Category="Inventory")
	FOnUpdateInventorySlots	OnUpdateInventorySlots;
	UPROPERTY(BlueprintAssignable, Category="Inventory")
	FOnUpdateCharacterSlot	OnUpdateCharacterSlot;

protected:

private:
	// Functions
	bool SwapCharacterHandToInventory(int To, int From);
	bool SwapInventoryToCharacterItem(int To, int From);
	bool SwapCharacterToInventoryItem(int To, int From);
	AFugitiveCharacter* GetPawn();


	// Values	
	AFugitiveCharacter* MyPawn = nullptr;
	
public:	
	UFUNCTION(BlueprintCallable)
	int AddItem(FItemInfo ItemInfo, uint8 BulletOnMagazine = 0);
	UFUNCTION(BlueprintCallable)
	void RemoveItem(uint8 IndexSlot);
	UFUNCTION(BlueprintCallable)
	EDropMessageType CanDropItem(uint8 ItemIndex);
	UFUNCTION(BlueprintCallable)
	void SpawnDroppedItem(FInventoryItemSlot InventoryItemSlot);
	UFUNCTION(BlueprintCallable)
	FInventoryItemSlot GetInfoItemBySlot(uint8 ItemIndex);



	UFUNCTION(BlueprintCallable)
	bool TryGetItem(FItemInfo ItemInfo);

	void UpdateWeaponItem(uint8 InventoryIndex, uint8 NewRound);
	bool GetWeaponFromHand(uint8 IndexSlot, FName &WeaponName, uint8 &RoundInMagazine);	
	bool SwapInventoryItems(int ItemIndex1, int ItemIndex2);
	bool CheckAmmoForWeapon(FName RoundTypeName);
	bool GetAmmoForWeapon(FName RoundTypeName, uint8 &AvailableAmmoForWeapon);
	void RemoveAmmo(FName RoundTypeName, int Num);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory|Items", meta = (AllowPrivateAccess = "true"))
	TArray<FInventoryItemSlot> Items;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory|Items", meta = (AllowPrivateAccess = "true"))
	uint8 MaxItemsBySlot = 255;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory|Items", meta = (AllowPrivateAccess = "true"))
	int ItemsForCharacterInventory = 6;

	// Flags
	
		
};
