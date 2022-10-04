// Fill out your copyright notice in the Description page of Project Settings.

#include "BaseEnemyCharacter.h"


#include "Components/CapsuleComponent.h"
#include "Fugitive/Core/MyGameInstance.h"
#include "Fugitive/Core/Components/T_HealthComponent.h"
#include "Fugitive/Objects/Items/DropItem.h"
#include "Fugitive/Weapon/WeaponDefault.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
ABaseEnemyCharacter::ABaseEnemyCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MouseCollision = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Mouse Collision"));
	MouseCollision->SetRelativeLocation(FVector::ZeroVector);
	MouseCollision->SetCapsuleHalfHeight(80.f);
	MouseCollision->SetCapsuleRadius(40.f);
	MouseCollision->SetCollisionProfileName("MouseOverlap", true);
	MouseCollision->SetupAttachment(RootComponent);	
	
	HealthComponent = CreateDefaultSubobject<UT_HealthComponent>(TEXT("HealthComponent"));

}

// Called when the game starts or when spawned
void ABaseEnemyCharacter::BeginPlay()
{
	Super::BeginPlay();

	GameInstance = Cast<UMyGameInstance>( GetGameInstance());
	if (!GameInstance)
	{
		UE_LOG(LogTemp, Error, TEXT("%s - Can't get game instance"), *__FUNCTION__);
		return;
	}
	
	if (!GameInstance->GetEnemyInfoByName(EnemyTableName, EnemyInfo))
	{
		UE_LOG(LogTemp, Error, TEXT("%s - Can't get enemy info"), *__FUNCTION__);
		return;
	}
	

	if (WeaponSlots[1].ItemName != NAME_None)
		TakeSelectWeapon(1);
	else if (WeaponSlots[0].ItemName != NAME_None)
		TakeSelectWeapon(0);


	if (HealthComponent)
	{
		HealthComponent->SetCurrentHealth(EnemyInfo.Health);
		HealthComponent->OnEmptyHealth.AddDynamic(this, &ABaseEnemyCharacter::OnEmptyHealth);		
	}
}

// Called every frame
void ABaseEnemyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABaseEnemyCharacter::TakeSelectWeapon(uint8 WeaponSlot)
{
	if (CurrentWeaponSlot == WeaponSlot)
		return;
	
	if (WeaponSlots.Num() < WeaponSlot && WeaponSlots[WeaponSlot].ItemName != NAME_None)
		return;
	
	if (IsValid(CurrentWeapon) && CurrentWeapon->IsReload())
		CurrentWeapon->CancelReload();

	// Один патрон в магазине, потому что они бесконечны
	CharacterInitWeapon(WeaponSlots[WeaponSlot].ItemName);
	CurrentWeaponSlot = WeaponSlot;

}

void ABaseEnemyCharacter::StopFire()
{
	if (IsValid(CurrentWeapon))
		CurrentWeapon->SetWeaponStateFire(false);	
}

void ABaseEnemyCharacter::Destroyed()
{
	TArray<AActor*> AttachedActors;
	GetAttachedActors(AttachedActors, true);
	for (int i=0; i<AttachedActors.Num(); i++)
		AttachedActors[i]->Destroy();
}

void ABaseEnemyCharacter::OnFire(UAnimSequence* Anim)
{
	if (!CurrentWeapon)
		return;
	
}

void ABaseEnemyCharacter::SetWeaponStateFire(bool bState)
{
	if (!CurrentWeapon)
		return;

	CurrentWeapon->SetWeaponStateFire(bState);
}

void ABaseEnemyCharacter::WeaponFire(AActor* EnemyActor)
{
	if (!IsValid(CurrentWeapon))
		return;

	/*
	if (CurrentWeaponSlot == 2) // Grenade
	{
		bCanThrowGrenade = false;

		// Если закончились гранаты прерываем стрельбу
		if (WeaponSlots[CurrentWeaponSlot].Count <= 0)
		{
			SetWeaponStateFire(false);
			CurrentWeapon->Destroy();
			return;
		}

		--WeaponSlots[CurrentWeaponSlot].Count;

		// Запускаем таймер, который снова включит возможность броска гранаты
		GetWorldTimerManager().SetTimer(EnableThrowGrenadeTimerHandle, this, &ABaseEnemyCharacter::EnableThrowGrenade,
                                        ThrowGrenadeReloadSec, false);
	}*/

	CurrentWeapon->TargetActor = EnemyActor;	
	SetWeaponStateFire(true);
}

void ABaseEnemyCharacter::GrenadeFire(AActor* EnemyActor)
{
	if (!IsValid(CurrentWeaponGrenade))
		return;

	bCanThrowGrenade = false;

	// Если закончились гранаты прерываем стрельбу
	if (WeaponSlots[CurrentWeaponSlot].Count <= 0)
	{
		SetWeaponStateFire(false);
		CurrentWeaponGrenade->Destroy();
		return;
	}

	--WeaponSlots[CurrentWeaponSlot].Count;

	// Запускаем таймер, который снова включит возможность броска гранаты
	GetWorldTimerManager().SetTimer(EnableThrowGrenadeTimerHandle, this, &ABaseEnemyCharacter::EnableThrowGrenade,
                                    ThrowGrenadeReloadSec, false);

	CurrentWeaponGrenade->TargetActor = EnemyActor;
	CurrentWeaponGrenade->SetWeaponStateFire(true);
}

bool ABaseEnemyCharacter::CanFireAttackPawn(FVector EnemyLocation)
{
	if (!CanFire())
		return false;

	FVector MyLocation = GetActorLocation();
	float Distance = (MyLocation - EnemyLocation).Size();

	return (Distance >= FireDistanceMin && Distance <= FireDistanceMax);	
}

bool ABaseEnemyCharacter::CanThrowGrenade(AActor* TargetActor)
{
	if (!TargetActor || !bCanAttackPawn || !bCanThrowGrenade || WeaponSlots.Num() < 2)
		return false;

	if (WeaponSlots[2].ItemName == NAME_None && WeaponSlots[2].Count < 1)
		return false;

	FVector MyLocation = GetActorLocation();
	FVector TargetLocation = TargetActor->GetActorLocation();
	float Distance = (TargetLocation - MyLocation).Size();

	if (Distance >= ThrowGrenadeMinDistance && Distance <= ThrowGrenadeMaxDistance)
	{
		float rnd = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);		
		if (rnd < ThrowGrenadeChanse)
			return true;
		else
		{
			bCanThrowGrenade = false;
			
			// Запускаем таймер, который снова включит возможность броска гранаты
			GetWorldTimerManager().SetTimer(EnableThrowGrenadeTimerHandle, this, &ABaseEnemyCharacter::EnableThrowGrenade,
                                            ThrowGrenadeReloadSec, false);
		}
	}
	

	return false;
}

void ABaseEnemyCharacter::SetCanMove(bool CanMove)
{
	bCanMove = CanMove;
	OnChangeCanMove.Broadcast(this, CanMove);

	if (GetCharacterMovement())
		if (!CanMove )
		{
			OldVelocity = GetCharacterMovement()->Velocity;
			GetCharacterMovement()->Velocity = FVector(0.f);
		}
		else
			GetCharacterMovement()->Velocity = OldVelocity;
}

bool ABaseEnemyCharacter::CanFire()
{	
	if (!bCanAttackPawn || !CurrentWeapon)
		return false;
	
	if (WeaponSlots[CurrentWeaponSlot].ItemName == NAME_None )
		return false;

	if (!CurrentWeapon->IsReload())
		return true;

	return false;
		
}

void ABaseEnemyCharacter::CharacterInitWeapon(FName IdWeaponName)
{
	FWeaponInfo myWeaponInfo;
//	UMyGameInstance* myGI = Cast<UMyGameInstance>(GetGameInstance());

	if (!GameInstance)// && !myGI->GetWeaponInfoByName(IdWeaponName, myWeaponInfo))
		return;
		
	if (!GameInstance->GetWeaponInfoByName(IdWeaponName, myWeaponInfo))
		return;

	if (IsValid(CurrentWeapon) && CurrentWeapon->WeaponInfo.WeaponClass == myWeaponInfo.WeaponClass)
	{
		if (CurrentWeaponName == IdWeaponName)
			return;
	}

	if (myWeaponInfo.WeaponType != EWeaponTypes::Grenade)
	{
		if (IsValid(CurrentWeapon))
		{
			CurrentWeapon->Destroy();
			CurrentWeapon = nullptr;
		}
		CurrentWeaponName = IdWeaponName;
	}
	
	
	if (myWeaponInfo.WeaponClass)
	{
		FVector SpawnLocation = FVector(0);
		FRotator SpawnRotation = FRotator(0);

		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		SpawnParams.Owner = this;
		SpawnParams.Instigator = GetInstigator();

		AWeaponDefault* myWeapon = Cast<AWeaponDefault>(GetWorld()
			->SpawnActor(myWeaponInfo.WeaponClass, &SpawnLocation, &SpawnRotation, SpawnParams));
		
		if (myWeapon)
		{
			myWeapon->SetEnemyFlag(true);
			myWeapon->OnWeaponFireStart.AddDynamic(this, &ABaseEnemyCharacter::OnFire);
			
/*			if (myWeaponInfo.WeaponType == EWeaponTypes::Grenade)
			{
				FAttachmentTransformRules Rule(EAttachmentRule::SnapToTarget, false);
				myWeapon->AttachToComponent(GetMesh(), Rule, FName("WeaponSocketRightHand"));

//				if (MyPlayerController)
//				{
//					myWeapon->ShootEndLocation = CursorLocation;
//				}
				
				float Distance = (myWeapon->ShootEndLocation - GetActorLocation()).Size();
				if (Distance < 400.f)
					myWeaponInfo.ProjectileSetting.ProjectileInitSpeed = Distance;
				else
					myWeaponInfo.ProjectileSetting.ProjectileInitSpeed = Distance/1.5;
				
				myWeapon->WeaponInfo = myWeaponInfo;
				
				UE_LOG(LogTemp, Warning, TEXT("dist: %f"), Distance);
				myWeapon->FireGrenade();
			}
			else
			{*/
//				FAttachmentTransformRules Rule(EAttachmentRule::SnapToTarget, false);
//				myWeapon->AttachToComponent(GetMesh(), Rule, FName("WeaponSocketRightHand"));
				myWeapon->InitWeapon(GetMesh(), myWeaponInfo, EMovementState::Aim_State, myWeaponInfo.MaxRoundInMagazine);
//				myWeapon->WeaponInfo = myWeaponInfo;
//				myWeapon->UpdateStateWeapon(EMovementState::Aim_State);				
//				myWeapon->SetWeaponRound(myWeaponInfo.MaxRoundInMagazine);

			if (myWeapon->WeaponInfo.WeaponType == EWeaponTypes::Grenade)
			{
				CurrentWeaponGrenade = myWeapon;
			}
			else
			{
				CurrentWeapon = myWeapon;
			}
/*			}*/
		}
	}
	else
		UE_LOG(LogTemp, Warning, TEXT("%s - Weapon not found in table (NULL)"), TEXT(__FUNCTION__));		

}

void ABaseEnemyCharacter::OnEmptyHealth()
{	
	OnEmptyHealtBP();

	if (CurrentWeapon)
	{
		CurrentWeapon->Destroy();
		CurrentWeapon = nullptr;
	}
	
	OnDropLoot();
	OnDead();
}

void ABaseEnemyCharacter::OnEmptyHealtBP_Implementation()
{
	// In BP
}

void ABaseEnemyCharacter::OnDead()
{
	bCanAttackPawn = false;
	bCanThrowGrenade = false;
	bCanMove = false;
	
	float TimeAnimation = 1.f; // 0.f; 
		
	int32 rnd = FMath::RandHelper(DeadsAnim.Num()); 
	if (DeadsAnim.IsValidIndex(rnd) && DeadsAnim[rnd] && GetMesh() && GetMesh()->GetAnimInstance())
	{
		GetMesh()->GetAnimInstance()->StopAllMontages(false);
		TimeAnimation = DeadsAnim[rnd]->GetPlayLength();
		GetMesh()->GetAnimInstance()->Montage_Play(DeadsAnim[rnd]);
	}
		
	UnPossessed();

	// Timer ragdoll
	GetWorldTimerManager().SetTimer(RagdollTimerHandle, this, &ABaseEnemyCharacter::EnableRagdoll,
                                        TimeAnimation, false);

}

void ABaseEnemyCharacter::EnableRagdoll()
{
	if (GetMesh())
	{
/*		RootComponent->SetAllMassScale(5000.f);
		GetCapsuleComponent()->SetMassScale(NULL, 5000.f);*/
		GetMesh()->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
		GetMesh()->SetAllMassScale(50000.f);
		GetMesh()->SetSimulatePhysics(true);
	}
}

void ABaseEnemyCharacter::EnableThrowGrenade()
{
	bCanThrowGrenade = true;
}

void ABaseEnemyCharacter::OnDropLoot()
{
	if (!GameInstance)
		return;

	FEnemyLootDropInfo* LootInfo = &EnemyInfo.EnemyLootDropInfo;

	// Static loot
	for (int i = 0; i < LootInfo->StaticLoot.Num(); i++)
	{
		SpawnRandomLoot(&LootInfo->StaticLoot[i]);
	}

	// Easy loot
	for (int i = 0; i < LootInfo->EasyLoot.Num(); i++)
	{
		if (UKismetMathLibrary::RandomInteger(100) <= LootInfo->EasyLootPercentChance)
		{
			SpawnRandomLoot(&LootInfo->MediumLoot[i]);
		}
	}

	// Medium loot
	for (int i = 0; i < LootInfo->MediumLoot.Num(); i++)
	{
		if (UKismetMathLibrary::RandomInteger(100) <= LootInfo->MediumLootPercentChance)
		{
			SpawnRandomLoot(&LootInfo->MediumLoot[i]);
		}
	}

	// Vip loot
	for (int i = 0; i < LootInfo->VipLoot.Num(); i++)
	{
		if (UKismetMathLibrary::RandomInteger(100) <= LootInfo->VipLootPercentChance)
		{
			SpawnRandomLoot(&LootInfo->VipLoot[i]);
		}
	}

}

void ABaseEnemyCharacter::SpawnRandomLoot(FDropLootInfo* LootInfo)
{
	
	if (GameInstance && LootInfo != nullptr)
	{
		FItemInfo ItemInfo;
		FLootInfo TmpLootInfo;
		if (!GameInstance->GetUnknownNameLootInfoByName(LootInfo->LootTableName, ItemInfo))
		{
			return;			
		}
		
		ItemInfo.Count = UKismetMathLibrary::RandomIntegerInRange(LootInfo->MinRndCout, LootInfo->MaxRndCout);

		FRotator SpawnRotation = FRotator(0);
		FVector SpawnLocation = GetActorLocation();

		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;
		SpawnParams.Owner = this;
		SpawnParams.Instigator = GetInstigator();

		ADropItem* DropItem = static_cast<ADropItem*>(GetWorld()->
                                        SpawnActor(ADropItem::StaticClass(), &SpawnLocation, &SpawnRotation, SpawnParams));
	
		if (DropItem)
			DropItem->Init(ItemInfo);
	}
}

