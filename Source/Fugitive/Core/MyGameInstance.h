// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine/DataTable.h"
#include "Engine/GameInstance.h"
#include "Fugitive/Types.h"
#include "Fugitive/Network/Interfaces/NetworkInterface.h"


#include "MyGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class FUGITIVE_API UMyGameInstance : public UGameInstance, public INetworkInterface
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Info Tables")
	UDataTable* WeaponInfoTable = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Info Tables")
	UDataTable* RoundInfoTable = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Info Tables")
	UDataTable* LootInfoTable = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Info Tables")
	UDataTable* EnemyInfoTable = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Info Tables")
	UDataTable* EnemyLootDropTable = nullptr;

	
	UFUNCTION(BlueprintCallable, Category = "Weapon")
    bool GetWeaponInfoByName(FName WeaponName, FWeaponInfo& OutInfo);
	UFUNCTION(BlueprintCallable, Category = "Round")
    bool GetRoundInfoByName(FName RoundName, FRoundInfo& OutInfo);
	UFUNCTION(BlueprintCallable, Category = "Loot")
    bool GetLootInfoByName(FName LootName, FLootInfo& OutInfo);
	UFUNCTION(BlueprintCallable, Category = "Enemy")
    bool GetEnemyInfoByName(FString EnemyName, FEnemyInfoTable& OutInfo);
	UFUNCTION(BlueprintCallable, Category = "Enemy")
    bool GetUnknownNameLootInfoByName(FString LootName, FItemInfo& OutInfo);


	// INetworkInterface
//	virtual void JoinNetworkSession(FOnlineSessionSearchResult& OnlineSessionSearchResult) override;
};
