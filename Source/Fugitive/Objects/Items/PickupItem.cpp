// Fill out your copyright notice in the Description page of Project Settings.


#include "PickupItem.h"
#include "Components/SphereComponent.h"
#include "Kismet/KismetMathLibrary.h"

#include "Fugitive/Core/MyGameInstance.h"

// Sets default values
APickupItem::APickupItem()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Scene"));
	RootComponent = SceneComponent;

	CollisionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("Collision Sphere"));
	CollisionSphere->SetSphereRadius(40.f);
	CollisionSphere->SetCollisionProfileName("MouseOverlap", true);
	CollisionSphere->SetupAttachment(RootComponent);

	ItemSkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Item Skeletal Mesh"));	
	ItemSkeletalMesh->SetupAttachment(RootComponent);
	ItemSkeletalMesh->SetCanEverAffectNavigation(false);
	ItemSkeletalMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	ItemStaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Item Static Mesh"));	
	ItemStaticMesh->SetupAttachment(RootComponent);
	ItemStaticMesh->SetCanEverAffectNavigation(false);
	ItemStaticMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	PostProcess = CreateDefaultSubobject<UPostProcessComponent>(TEXT("Post Process"));	
	PostProcess->SetupAttachment(RootComponent);
	PostProcess->bUnbound = false;
	PostProcess->bEnabled = true;
	
	static ConstructorHelpers::FObjectFinder<UObject> InteractableObjectHoverMaterialFinder(TEXT("MaterialInstanceConstant'/Game/TODEL/Materials/PP_Outliner_Inst.PP_Outliner_Inst'"));
	ExcretionMaterial= Cast<UMaterialInstance>(InteractableObjectHoverMaterialFinder.Object);
	FPostProcessSettings& PostProcessSettings = PostProcess->Settings;
	FWeightedBlendable WeightedBlendable;
	WeightedBlendable.Object = ExcretionMaterial;
	WeightedBlendable.Weight = 1;
	PostProcessSettings.WeightedBlendables.Array.Add(WeightedBlendable);
}

// Called when the game starts or when spawned
void APickupItem::BeginPlay()
{
	Super::BeginPlay();

/*	if (PostProcess)
	{
		UMaterialInstanceDynamic* DynamicMaterial = UMaterialInstanceDynamic::Create(Cast<UMaterialInstance>(ExcretionMaterial), GetWorld());
		if (DynamicMaterial)
		{
			PostProcessSettings = PostProcess->Settings;
			FWeightedBlendable WeightedBlendable;
			WeightedBlendable.Object = DynamicMaterial;
			WeightedBlendable.Weight = 1;
			PostProcessSettings.WeightedBlendables.Array.Add(WeightedBlendable);
		}
	}
*/
	GameInstance = Cast<UMyGameInstance>( GetGameInstance());
	if (!GameInstance)
	{
		UE_LOG(LogTemp, Error, TEXT("%s - Can't get game instance"), *__FUNCTION__);
		return;
	}

	if (!bIsDropItem)
	{
		VisualMeshInit();
		CheckValidMeshes();
		AmountRandom();
	}
}

void APickupItem::VisualMeshInit()
{
	UStaticMesh* NewMesh = nullptr;
	
	if (!ItemInfo.ItemName.IsNone() && !bIsBPItem)
	{
		if (ItemInfo.ItemType == EItemType::WeaponItem)
		{
			FWeaponInfo Weapon;
			if (!GameInstance->GetWeaponInfoByName(ItemInfo.ItemName, Weapon))
				return;

			NewMesh = Weapon.LootItemMesh;
		}
		else if (ItemInfo.ItemType == EItemType::RoundItem)
		{
			FRoundInfo Round;
			if (!GameInstance->GetRoundInfoByName(ItemInfo.ItemName, Round))
				return;

			NewMesh = Round.LootItemMesh;
		}
		else if (ItemInfo.ItemType == EItemType::LootItem) {
			FLootInfo Loot;
			if (!GameInstance->GetLootInfoByName(ItemInfo.ItemName, Loot))
				return;

			NewMesh = Loot.LootItemMesh;
		}

		if (NewMesh)
		{
			ItemStaticMesh->SetStaticMesh(NewMesh);
			ItemSkeletalMesh->DestroyComponent();
		}
	}
}

// Проверяет заргружен ли какой-нибудь мешь, а пустые удаляет
void APickupItem::CheckValidMeshes()
{
	bool bIsOnePreset = true;
	
	if (!ItemSkeletalMesh->MeshObject)
	{
		ItemSkeletalMesh->DestroyComponent();
		bIsOnePreset = false;
	}
	
	if (!ItemStaticMesh->GetStaticMesh())
	{
		if (!bIsOnePreset)
			this->Destroy();
		else
			ItemStaticMesh->DestroyComponent();	
	}
}

void APickupItem::AmountRandom()
{
	if (bRandom && ItemInfo.Count > 1)
	{
		ItemInfo.Count = UKismetMathLibrary::RandomIntegerInRange(1, ItemInfo.Count);
	}
}

void APickupItem::SetCount(uint8 Count)
{
	if (Count <= 0)
		OnKill();
	else
		ItemInfo.Count = Count;
}

void APickupItem::OnKill()
{
	this->Destroy();	
}

void APickupItem::CustomDephForSelectEnable_Implementation(bool bIsEnable)
{
	if (IsValid(ItemSkeletalMesh))
		ItemSkeletalMesh->SetRenderCustomDepth(bIsEnable);

	if (IsValid(ItemStaticMesh))
		ItemStaticMesh->SetRenderCustomDepth(bIsEnable);
}

bool APickupItem::HaveYouCustomDephForSelect_Implementation()
{
	return true;
}

EObjectAction APickupItem::HowAreYou_Implementation(AActor* Actor)
{
	return EObjectAction::Pickup;
}

FItemInfo APickupItem::WhatIGet_Implementation(AActor* Actor)
{
	return ItemInfo;
}

void APickupItem::SetAmount_Implementation(AActor* Actor, uint8 Amount)
{
	SetCount(Amount);
}


