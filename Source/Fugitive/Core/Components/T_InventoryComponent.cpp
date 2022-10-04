// Fill out your copyright notice in the Description page of Project Settings.


#include "Fugitive/Core/Components/T_InventoryComponent.h"
#include "Fugitive/Player/FugitiveCharacter.h"
#include "Fugitive/Core/MyGameInstance.h"
#include "Fugitive/Objects/Items/DropItem.h"
#include "Kismet/GameplayStatics.h"

// Sets default values for this component's properties
UT_InventoryComponent::UT_InventoryComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}

AFugitiveCharacter* UT_InventoryComponent::GetPawn()
{
	if (!MyPawn && GetOwner())
		MyPawn = Cast<AFugitiveCharacter>(GetOwner());

	return MyPawn;
}

void UT_InventoryComponent::SpawnDroppedItem(FInventoryItemSlot InventoryItemSlot)
{
	FRotator SpawnRotation = FRotator(0);
	FVector SpawnLocation = GetOwner()->GetActorLocation();

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;
	SpawnParams.Owner = GetOwner();
	SpawnParams.Instigator = GetOwner()->GetInstigator();

	ADropItem* DropItem = static_cast<ADropItem*>(GetWorld()->
                                    SpawnActor(ADropItem::StaticClass(), &SpawnLocation, &SpawnRotation, SpawnParams));
	
	if (DropItem)
		DropItem->Init(InventoryItemSlot.ItemInfo);
}

FInventoryItemSlot UT_InventoryComponent::GetInfoItemBySlot(uint8 ItemIndex)
{
	FInventoryItemSlot ItemSlot;
	
	if (Items.Num() <= 0 && Items.Num() < ItemIndex)
		return ItemSlot;

	return Items[ItemIndex];
}

bool UT_InventoryComponent::TryGetItem(FItemInfo ItemInfo)
{
	if (ItemInfo.ItemName == NAME_None)
		return false;

	bool bIsAlreadyPresent = false;
	int8 i = ItemsForCharacterInventory;

	if (Items.Num() <= ItemsForCharacterInventory)
		return false;	
	
	while (i < Items.Num() && !bIsAlreadyPresent)
	{
		if ((Items[i].ItemInfo.ItemName == ItemInfo.ItemName && Items[i].ItemInfo.Count < MaxItemsBySlot && !Items[i].IsOneItemBySlot)
			|| Items[i].ItemInfo.ItemName == NAME_None)
		{
			return true;
		}
		++i;
	}

	return false;
}

void UT_InventoryComponent::UpdateWeaponItem(uint8 InventoryIndex, uint8 NewRound)
{
	if (Items.Num() >= InventoryIndex)
		Items[InventoryIndex].BulletOnMagazine = NewRound;
		OnUpdateCharacterSlot.Broadcast(InventoryIndex);
}

bool UT_InventoryComponent::GetWeaponFromHand(uint8 IndexSlot, FName& WeaponName, uint8& RoundInMagazine)
{
	bool bResult = false;
	if (Items.Num() >= IndexSlot && Items[IndexSlot].ItemInfo.ItemName != NAME_None)
	{
		WeaponName = Items[IndexSlot].ItemInfo.ItemName;
		RoundInMagazine = Items[IndexSlot].BulletOnMagazine;
		UE_LOG(LogTemp, Warning, TEXT("%s: Name=%s, Round=%d"), TEXT(__FUNCTION__), *WeaponName.ToString(), RoundInMagazine);
		bResult = true;
	}

	return bResult;
}

int UT_InventoryComponent::AddItem(FItemInfo ItemInfo, uint8 BulletOnMagazine)
{
	if (ItemInfo.ItemName == NAME_None)
		return 0;

	// Find item in to the inventory slots
	int8 i = ItemsForCharacterInventory;
	int8 FirstEmptyItem = -1;
	if (Items.Num() <= ItemsForCharacterInventory)
		return 0;
	
	while (i < Items.Num() )
	{
		if (Items[i].ItemInfo.ItemName == ItemInfo.ItemName
			&& Items[i].ItemInfo.Count < MaxItemsBySlot
			&& !Items[i].IsOneItemBySlot) // Count == uint8
		{
			uint8 EmptyItemsBySlot = MaxItemsBySlot - Items[i].ItemInfo.Count;
			if (EmptyItemsBySlot >= ItemInfo.Count)
			{
				Items[i].ItemInfo.Count += ItemInfo.Count;
				OnUpdateInventorySlots.Broadcast();
				return 0;
			}
			else
			{
				Items[i].ItemInfo.Count += EmptyItemsBySlot;
				ItemInfo.Count -= EmptyItemsBySlot;
			}
		}
			
		else if (Items[i].ItemInfo.ItemName == NAME_None && FirstEmptyItem == -1)
			FirstEmptyItem = i;
		
		i++;
	}

	bool IsOneItemBySlot = false;

	if (ItemInfo.ItemType == EItemType::WeaponItem)
	{
		IsOneItemBySlot = true;
		Items[FirstEmptyItem].BulletOnMagazine = BulletOnMagazine;
	}

	if (ItemInfo.Count > 0 && FirstEmptyItem != -1)
	{
		Items[FirstEmptyItem].ItemInfo.ItemName = ItemInfo.ItemName;
		Items[FirstEmptyItem].ItemInfo.Count = ItemInfo.Count;
		Items[FirstEmptyItem].ItemInfo.ItemType = ItemInfo.ItemType;
		Items[FirstEmptyItem].IsOneItemBySlot = IsOneItemBySlot;
		

		ItemInfo.Count = 0;
	}

	OnUpdateInventorySlots.Broadcast();
	
	return ItemInfo.Count;
}

EDropMessageType UT_InventoryComponent::CanDropItem(uint8 ItemIndex)
{
	if (Items.Num() <= 0 && Items.Num() < ItemIndex)
		return EDropMessageType::Unknown;
	
	//todo: в первую очередь должны проверить является ли предмет квестовым

	// Запрос для оружия и патронов
	if (Items[ItemIndex].ItemInfo.ItemType == EItemType::WeaponItem
		|| Items[ItemIndex].ItemInfo.ItemType == EItemType::RoundItem)
	{
		return EDropMessageType::Request;
	}

	// Нет запроса для обычного лута
	return EDropMessageType::Thrown;
}

void UT_InventoryComponent::RemoveItem(uint8 IndexSlot)
{
	if (Items.Num() <= 0 && Items.Num() < IndexSlot)
		return;

	Items[IndexSlot].ItemInfo.Count = 0;
	Items[IndexSlot].BulletOnMagazine = 0;
	Items[IndexSlot].ItemInfo.ItemName = NAME_None;
	OnUpdateInventorySlots.Broadcast();
	OnUpdateCharacterSlot.Broadcast(IndexSlot);
	
}

bool UT_InventoryComponent::SwapCharacterHandToInventory(int To, int From)
{
	bool bIsResult = false;
	
	if (GetPawn())
	{
		if (TryGetItem(Items[From].ItemInfo) && GetPawn())
		{
			AddItem(Items[From].ItemInfo, MyPawn->GetWeaponRound());
			Items[From].BulletOnMagazine = 0;
			
			if (GetPawn())
			{
				Items[From].BulletOnMagazine = MyPawn->GetWeaponRound();
				MyPawn->BreakWeaponHead(From);
			}
			else
				UE_LOG(LogTemp, Error, TEXT("%s: Can't find pawn"), TEXT(__FUNCTION__));
			
			
			Items[From].ItemInfo.Count = 1;			
			Items[From].ItemInfo.ItemName = NAME_None;
			bIsResult = true;	
		}
		//todo: else тут надо вывести сообщение, что инвентарь переполнен
	}
	return bIsResult;
}

bool UT_InventoryComponent::SwapInventoryToCharacterItem(int To, int From)
{
	bool bIsResult = false;

	UMyGameInstance* GI = Cast<UMyGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	if (!GI)
		return bIsResult;

	// Обмен предметами между руками
	if (To <= 1 && From <= 1)
	{
		if (GetPawn())
		{
			Items[From].BulletOnMagazine = MyPawn->GetWeaponRound();
			MyPawn->BreakWeaponHead(From);
		}
		else
			UE_LOG(LogTemp, Error, TEXT("%s: Can't find pawn"), TEXT(__FUNCTION__));
		
		Swap(Items[To], Items[From]);
		OnUpdateCharacterSlot.Broadcast(From);		
		bIsResult = true;
	}
	// Обмен предметами на поясе
	else if (To > 1 && To < ItemsForCharacterInventory
		&& From > 1 && From < ItemsForCharacterInventory)
	{
		Swap(Items[To], Items[From]);
		OnUpdateCharacterSlot.Broadcast(From);
		bIsResult = true;
	}
	// Обмен предметами между поясом и руками
	else if (To < ItemsForCharacterInventory && From < ItemsForCharacterInventory)
	{
		bIsResult = false;
	}
	// Обмен между инвентарем и руками
	else if (To <= 1)
	{
		
		if (Items[From].ItemInfo.ItemType == EItemType::WeaponItem)
		{
			FWeaponInfo WeaponInfo;
			GI->GetWeaponInfoByName(Items[From].ItemInfo.ItemName, WeaponInfo);
			if (WeaponInfo.WeaponType != EWeaponTypes::Grenade)
			{
				if (Items[To].ItemInfo.ItemName == NAME_None)
				{
					Items[To] = Items[From];
					Items[From].BulletOnMagazine = 0;
					Items[From].ItemInfo.Count = 1;
					Items[From].ItemInfo.ItemName = NAME_None;
				}
				else
					Swap(Items[To], Items[From]);

				bIsResult = true;
			}
		}
	}
	// Belt slots
	else
	{
		if (Items[From].ItemInfo.ItemType == EItemType::WeaponItem) // В дальнейшем + лечащие предметы
		{
			FWeaponInfo WeaponInfo;
			GI->GetWeaponInfoByName(Items[From].ItemInfo.ItemName, WeaponInfo);
			if (WeaponInfo.WeaponType == EWeaponTypes::Grenade)
			{
				uint8 Count = 0;
				
				for (int i = ItemsForCharacterInventory; i< Items.Num() - ItemsForCharacterInventory; i++)
				{
					if (Items[i].ItemInfo.ItemName == Items[From].ItemInfo.ItemName)
					{
						Count += Items[i].ItemInfo.Count;
					}
				}

				if (Count > MaxItemsBySlot)
					Count = MaxItemsBySlot;


				Items[To] = Items[From];
				Items[To].ItemInfo.Count = Count;

				bIsResult = true;
			}
		}
	}

	if (bIsResult)
	{
		OnUpdateInventorySlots.Broadcast();
		OnUpdateCharacterSlot.Broadcast(To);
	}

	return bIsResult;
}

bool UT_InventoryComponent::SwapCharacterToInventoryItem(int To, int From)
{
	bool bIsResult = false;

	UMyGameInstance* GI = Cast<UMyGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	if (!GI)
		return bIsResult;

	// Обмен между руками и инвентарем
	if (From <= 1)
	{
		bIsResult = SwapCharacterHandToInventory(To, From);

	}
	// Belt slots
	else
	{
		Items[From].ItemInfo.Count = 1;
		Items[From].BulletOnMagazine = 0;
		Items[From].ItemInfo.ItemName = NAME_None;
		bIsResult = true;		
	}

	if (bIsResult)
	{
		OnUpdateInventorySlots.Broadcast();
		OnUpdateCharacterSlot.Broadcast(From);
	}

	return bIsResult;
}

bool UT_InventoryComponent::SwapInventoryItems(int ItemIndex1, int ItemIndex2)
{
	if (ItemIndex1 == ItemIndex2)
		return true;

	if (Items.Num() < ItemIndex1 || Items.Num() < ItemIndex2)
		return false;
	
	// swap item Inventory to character
	if (ItemIndex1 < ItemsForCharacterInventory)
	{
		return SwapInventoryToCharacterItem(ItemIndex1, ItemIndex2);
	}

	// swap item character to Inventory
	if (ItemIndex2 < ItemsForCharacterInventory)
	{
		return SwapCharacterToInventoryItem(ItemIndex1, ItemIndex2);
	}

	// Если два объекта одинаковые, то пробуем их объединить
	if ((!Items[ItemIndex1].IsOneItemBySlot && !Items[ItemIndex2].IsOneItemBySlot)
		&& Items[ItemIndex1].ItemInfo.ItemName == Items[ItemIndex2].ItemInfo.ItemName
		&& Items[ItemIndex1].ItemInfo.Count < MaxItemsBySlot
		&& Items[ItemIndex2].ItemInfo.Count > 1)
	{
		uint8 MaxOver = MaxItemsBySlot - Items[ItemIndex1].ItemInfo.Count;

		if (MaxOver >= Items[ItemIndex2].ItemInfo.Count)
		{
			// Уничтожаем старый объект, т.к. все поместится в новый
			Items[ItemIndex1].ItemInfo.Count += Items[ItemIndex2].ItemInfo.Count;
			Items[ItemIndex2].ItemInfo.ItemName = NAME_None;
			Items[ItemIndex2].ItemInfo.Count = 0;
			Items[ItemIndex2].BulletOnMagazine = 0;
		}
		else
		{
			Items[ItemIndex1].ItemInfo.Count += MaxOver;
			Items[ItemIndex2].ItemInfo.Count -= MaxOver;
		}
	}
	else
		Swap(Items[ItemIndex1], Items[ItemIndex2]);		

	OnUpdateInventorySlots.Broadcast();
	
	
	return true;
}

bool UT_InventoryComponent::CheckAmmoForWeapon(FName RoundTypeName)
{
	if (RoundTypeName.IsNone())
		return false;

	for (int i=0; i < Items.Num(); i++)
	{
		if (Items[i].ItemInfo.ItemType == EItemType::RoundItem && Items[i].ItemInfo.ItemName == RoundTypeName)
		{
			return true;			
		}
	}
	
	return false;
}

bool UT_InventoryComponent::GetAmmoForWeapon(FName RoundTypeName, uint8& AvailableAmmoForWeapon)
{
	if (RoundTypeName.IsNone())
		return false;

	AvailableAmmoForWeapon = 0;

	for (int i=0; i < Items.Num(); i++)
	{
		if (Items[i].ItemInfo.ItemType == EItemType::RoundItem
			&& Items[i].ItemInfo.ItemName == RoundTypeName)
		{
			AvailableAmmoForWeapon += Items[i].ItemInfo.Count;
		}
	}

	if (AvailableAmmoForWeapon == 0)
		return false;
	
	return true;
}

void UT_InventoryComponent::RemoveAmmo(FName RoundTypeName, int Num)
{
	uint8 CurrentCount = Num;
	int i = 0;

	while (i < Items.Num())
	{
		if (Items[i].ItemInfo.ItemType == EItemType::RoundItem
            && Items[i].ItemInfo.ItemName == RoundTypeName)
		{
			if (Items[i].ItemInfo.Count <= CurrentCount)
			{
				CurrentCount -= Items[i].ItemInfo.Count;
				Items[i].ItemInfo.ItemName = NAME_None;
			}
			else
			{
				Items[i].ItemInfo.Count -= CurrentCount;
				break;
			}
		}
		++i;
	}
	
	OnUpdateInventorySlots.Broadcast();
}
