// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "Engine/DataTable.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Types.generated.h"

UENUM(BlueprintType)
enum class EObjectAction : uint8
{
	Pickup			UMETA(DisplayName = "Pickup"),
    Interaction		UMETA(DisplayName = "Interaction"),
	None			UMETA(DisplayName = "None"),
	InteractNPC		UMETA(DisplayName = "InteractNPC"),
	Enemy			UMETA(DisplayName = "Enemy"),
	//talk
	//...
};

UENUM(BlueprintType)
enum class EMinimapObjectType : uint8
{
	SimpleEnemy		UMETA(DisplayName = "Simple Enemy"),
    NPC				UMETA(DisplayName = "NPC"),
	Pickup			UMETA(DisplayName = "Pickup"),
};

UENUM(BlueprintType)
enum class EMovementState : uint8
{
	Aim_State			UMETA(DisplayName = "Aim State"),
	Walk_State			UMETA(DisplayName = "Walk State"),
	Backwalk_State		UMETA(DisplayName = "Backwalk State"),
	Sprint_State		UMETA(DisplayName = "Sprint State"),
	Crouch_State		UMETA(DisplayName = "Crouch State"),
	Jump_State			UMETA(DisplayName = "Jump State")
};


/* @Tahoma. Сообщения о том, можно ли выкинуть предмет */
UENUM(BlueprintType)
enum class EDropMessageType : uint8
{
	Unknown			UMETA(DisplayName = "Unknown"),
	Thrown			UMETA(DisplayName = "Can be thrown out"),
    Request			UMETA(DisplayName = "Can be thrown out by request"),
    Away			UMETA(DisplayName = "Can't be thrown away"),
};

// Состояние передвижения персонажа
USTRUCT(BlueprintType)
struct FCharacterSpeed
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float AimSpeed			= 300.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float WalkSpeed			= 600.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float BackwalkSpeed		= 400.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float SprintSpeed		= 800.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float CrouchSpeed		= 400.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float JumpSpeed			= 700.0f;

};

// Информация о активном предмете
USTRUCT(BlueprintType)
struct FActionItemInfo : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "General")
	bool DestroyAfterUse = true;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "General")
	UStaticMesh* StaticMesh = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "General")
	FRotator Rotate = FRotator::ZeroRotator;	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "General")
	FVector Scale = FVector(1.f);
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "General")
	UTexture2D* WidgetIcon = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Configure")
	FName ApplicableToActorName; 
};


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Loot
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
USTRUCT(BlueprintType)
struct FLootInfo : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Info")
	FString Title;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Info")
	FText Description;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh")
	UStaticMesh* LootItemMesh = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	UTexture2D* InventoryIcon = nullptr;
};

/* @Tahoma. Структура шанса выпадения лута и его вида */
USTRUCT(BlueprintType)
struct FDropLootInfo
{
	GENERATED_USTRUCT_BODY()

    /* Название лута из DataTable */
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString LootTableName;
	/* Минимальное количество, которое может выпасть */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	uint8 MinRndCout = 1;
	/* Максимальное количество, которое может выпасть */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	uint8 MaxRndCout = 1;	
};

/* @Tahoma. Информация о выпадающем луте. Можно настроить 3 разновидности шанса выпадения + 1, который падает всегда
 * - обычно простой, не нужный лут
 * - полезный лут
 * - вип-лут
 * - лут, который падает обязательно
 */
USTRUCT(BlueprintType)
struct FEnemyLootDropInfo
{
	GENERATED_USTRUCT_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Static loot")
	TArray<FDropLootInfo> StaticLoot;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Easy loot")
	TArray<FDropLootInfo> EasyLoot;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Easy loot")
	int EasyLootPercentChance = 100;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Medium loot")
	TArray<FDropLootInfo> MediumLoot;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Medium loot")
	int MediumLootPercentChance = 50;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vip loot")
	TArray<FDropLootInfo> VipLoot;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vip loot")
	int VipLootPercentChance = 25;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Inventory
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
UENUM(BlueprintType)
enum class EItemType : uint8
{
	WeaponItem		UMETA(DisplayName = "WeaponItem"),
	RoundItem		UMETA(DisplayName = "RoundItem"),
	LootItem		UMETA(DisplayName = "LootItem"),
};

USTRUCT(BlueprintType)
struct FItemInfo
{
	GENERATED_USTRUCT_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action Item")
	FName ItemName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Type")
	EItemType ItemType = EItemType::LootItem;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Info")
	uint8 Count = 1;

	FItemInfo() {};
	FItemInfo(FName InItemName, EItemType InItemType, uint8 InCount)
		: ItemName(InItemName), ItemType(InItemType), Count(InCount) {};
};

USTRUCT(BlueprintType)
struct FInventoryItemSlot 
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item info")
	FItemInfo ItemInfo;

	// Количество патронов заряженных в магазине (только для оружия)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	uint8 BulletOnMagazine = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	bool IsOneItemBySlot = false;

	FInventoryItemSlot()
	{
		ItemInfo = FItemInfo();
		BulletOnMagazine = 0;
		IsOneItemBySlot = false;		
	}
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Weapon
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
UENUM(BlueprintType)
// Определяет, как будет отображаться анимация персонажа с оружием в руках
enum class EWeaponTypes : uint8 
{
	Hand		UMETA(DisplayName = Hand),
    Projectile	UMETA(DisplayName = Projectile),
    Trace		UMETA(DisplayName = Trace),
    Grenade		UMETA(DisplayName = Grenade),
};

USTRUCT(BlueprintType)
struct FRoundInfo : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Info")
	FString Title;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Info")
	FText Description;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh")
	UStaticMesh* LootItemMesh = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	UTexture2D* InventoryIcon = nullptr;
};

USTRUCT(BlueprintType)
struct FWeaponDispersion
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dispersion | Aim")
	float Aim_StateDispersionAimMax = 2.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dispersion | Aim")
	float Aim_StateDispersionAimMin = 0.3f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dispersion | Aim")
	float Aim_StateDispersionAimRecoil = 1.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dispersion | Aim")
	float Aim_StateDispersionReduction = .3f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dispersion | Walk")
	float Walk_StateDispersionAimMax = 5.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dispersion | Walk")
	float Walk_StateDispersionAimMin = 1.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dispersion | Walk")
	float Walk_StateDispersionAimRecoil = 1.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dispersion | Walk")
	float Walk_StateDispersionReduction = 0.2f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dispersion | Walk")
	float Sprint_StateDispersionAimMax = 5.5f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dispersion | Walk")
	float Sprint_StateDispersionAimMin = 2.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dispersion | Walk")
	float Sprint_StateDispersionAimRecoil = 1.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dispersion | Walk")
	float Sprint_StateDispersionReduction = 0.2f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dispersion | Crouch")
	float Crouch_StateDispersionAimMax = 2.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dispersion | Crouch")
	float Crouch_StateDispersionAimMin = 0.3f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dispersion | Crouch")
	float Crouch_StateDispersionAimRecoil = 1.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dispersion | Crouch")
	float Crouch_StateDispersionReduction = .3f;
};

USTRUCT(BlueprintType)
struct FProjectileInfo
{
	GENERATED_USTRUCT_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile class")
	TSubclassOf<class AWeaponProjectile> Projectile = nullptr;

	// Вид вылетающего патрона
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ProjectileSetting")
	UStaticMesh* BulletMesh = nullptr;
	// Изменение размера патрона
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ProjectileSetting")
	FVector BulletScale = FVector(1.0f, 1.0f, 1.0f);
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ProjectileSetting")
	UParticleSystem* BulletFX = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ProjectileSetting")
	bool ShouldBounce = true;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ProjectileSetting")
	float ProjectileGravityScale = 0.1f;
//	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ProjectileSetting")
//	bool BulletPhysicSimulate = false;

	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ProjectileSetting")
	float ProjectileDamage = 20.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ProjectileSetting")
	float ProjectileLifeTime = 20.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ProjectileSetting")
	float ProjectileInitSpeed = 2000.0f;

	//material to decal on hit
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FX")
	TMap<TEnumAsByte<EPhysicalSurface>, UMaterialInterface*> HitDecals;
	//Sound when hit
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FX")
	USoundBase* HitSound = nullptr;
	//fx when hit check by surface
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<TEnumAsByte<EPhysicalSurface>, UParticleSystem*> HitFXs;

//	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect")
//	TSubclassOf<UTDS_StateEffect> Effect = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Explose")
	UParticleSystem* ExploseFX = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Explose")
	USoundBase* ExploseSound = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Explose")
	float ExploseStrongRadiusDamage = 80.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Explose")
	float ExploseWeakRadiusDamage = 120.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Explose")
	float ExploseCoefDamage = 10.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Explose")
	float ExploseMaxDamage = 500.0f;

};

USTRUCT(BlueprintType)
struct FWeaponInfo : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

	// Класс оружия
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Class")
	TSubclassOf<class AWeaponDefault> WeaponClass = nullptr;

	// Тип оружия
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Type")
	EWeaponTypes WeaponType;
	// Тип патронов
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Type")
	FName RoundTypeName;

	// Название оружия
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Info")
	FString Title;
	// Описание оружия
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Info")
	FText Description;

	// Настройки снаряда
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile")
	FProjectileInfo ProjectileSetting;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trace")
	float DistanceTrace = 2000.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trace")
	UParticleSystem* LaserEffect = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	float WeaponDamage = 20.0f; //TODO: это же значение надо передать в projectile
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	float ShotSpeed = 0.5f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	float ReloadTime = 3.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	uint8 RoundProjectileByShot = 1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	uint8 MaxRoundInMagazine = 2;

	// Настройки разброса
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dispersion")
	FWeaponDispersion DispersionWeapon;

	// Вид оружия в виде подбираемого предмета
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Meshes")
	UStaticMesh* LootItemMesh = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Meshes")
	UStaticMesh* EmptyBulletMesh = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Anim | Weapon")
	UAnimSequence* AnimWeaponFire = nullptr;

	// Звук при выстреле
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
	class UFMODEvent* StartFireSound;
	//USoundBase* SFXStartFire = nullptr;
	// Эффект при выстреле
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
	UParticleSystem* VFXStartFire = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	UTexture2D* InventoryIcon = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	UTexture2D* PlayerIcon = nullptr;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Enemy
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
USTRUCT(BlueprintType)
struct FEnemyInfoTable : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Health = 100.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FEnemyLootDropInfo EnemyLootDropInfo; 
};


UCLASS()
class FUGITIVE_API UTypes : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

};
