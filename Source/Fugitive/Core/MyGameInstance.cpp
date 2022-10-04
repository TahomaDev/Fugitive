// Fill out your copyright notice in the Description page of Project Settings.


#include "Fugitive/Core/MyGameInstance.h"

bool UMyGameInstance::GetWeaponInfoByName(FName WeaponName, FWeaponInfo& OutInfo)
{
	bool bIsFind = false;
	FWeaponInfo* WeaponInfoRow;

	if (WeaponInfoTable)
	{
		WeaponInfoRow = WeaponInfoTable->FindRow<FWeaponInfo>(WeaponName, "", false);
		if (WeaponInfoRow)
		{
			bIsFind = true;
			OutInfo = *WeaponInfoRow;
		}
	}
	else
		UE_LOG(LogTemp, Error, TEXT("UMyGameInstance::GetWeaponInfoByName - WeaponTable (NULL)"));
		
	return bIsFind;
}

bool UMyGameInstance::GetRoundInfoByName(FName RoundName, FRoundInfo& OutInfo)
{
	bool bIsFind = false;
	FRoundInfo* RoundInfoRow;

	if (RoundInfoTable)
	{
		RoundInfoRow = RoundInfoTable->FindRow<FRoundInfo>(RoundName, "", false);
		if (RoundInfoRow)
		{
			bIsFind = true;
			OutInfo = *RoundInfoRow;
		}
	}
	else
		UE_LOG(LogTemp, Error, TEXT("UMyGameInstance::GetRoundInfoByName - RoundTable (NULL)"));
		
	return bIsFind;
}

bool UMyGameInstance::GetLootInfoByName(FName LootName, FLootInfo& OutInfo)
{
	bool bIsFind = false;
	FLootInfo* LootInfoRow;

	if (LootInfoTable)
	{
		LootInfoRow = LootInfoTable->FindRow<FLootInfo>(LootName, "", false);
		if (LootInfoRow)
		{
			bIsFind = true;
			OutInfo = *LootInfoRow;
		}
	}
	else
		UE_LOG(LogTemp, Error, TEXT("UMyGameInstance::GetLootInfoByName - LootTable (NULL)"));
		
	return bIsFind;
}

bool UMyGameInstance::GetEnemyInfoByName(FString EnemyName, FEnemyInfoTable& OutInfo)
{
	bool bIsFind = false;
	UE_LOG(LogTemp, Warning, TEXT("UMyGameInstance::GetEnemyInfoByName - %s "), *EnemyName);
	FName FindName = FName(*EnemyName);
	FEnemyInfoTable* EnemyInfoRow;

	if (EnemyInfoTable)
	{
		EnemyInfoRow = EnemyInfoTable->FindRow<FEnemyInfoTable>(FindName, "", false);
		if (EnemyInfoRow)
		{
			bIsFind = true;
			OutInfo = *EnemyInfoRow;
		}
		else
			UE_LOG(LogTemp, Warning, TEXT("UMyGameInstance::GetEnemyInfoByName - %s can't find table"), *EnemyName);
	}
	else
		UE_LOG(LogTemp, Error, TEXT("UMyGameInstance::GetEnemyInfoByName - EnemyInfoTable (NULL)"));
		
	return bIsFind;
}

bool UMyGameInstance::GetUnknownNameLootInfoByName(FString LootName, FItemInfo& OutInfo)
{
	FName LootFName = FName(*LootName);
	
	FLootInfo TmpLootInfo;
	FRoundInfo TmpRoundInfo;
	FWeaponInfo TmpWeaponInfo;

	if (GetLootInfoByName(LootFName, TmpLootInfo))
	{
		OutInfo.ItemType = EItemType::LootItem;
		OutInfo.ItemName = LootFName;
		return true;
	}
	else if (GetRoundInfoByName(LootFName, TmpRoundInfo))
	{
		OutInfo.ItemType = EItemType::RoundItem;
		OutInfo.ItemName = LootFName;
		return true;
	}
	else if (GetWeaponInfoByName(LootFName, TmpWeaponInfo))
	{
		OutInfo.ItemType = EItemType::WeaponItem;
		OutInfo.ItemName = LootFName;
		return true;
	}

	return false;
}
