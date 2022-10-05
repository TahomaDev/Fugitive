// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponDefault.h"
#include "DrawDebugHelpers.h"
#include "Components/ArrowComponent.h"
#include "Kismet/GameplayStatics.h"
#include "WeaponProjectile.h"
#include "Engine/StaticMeshActor.h"
#include "Fugitive/Core/Components/T_InventoryComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Perception/AISense_Damage.h"
//#include "FMODBlueprintStatics.h"

// Sets default values
AWeaponDefault::AWeaponDefault()
{
	PrimaryActorTick.bCanEverTick = true;

	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Scene"));
	RootComponent = SceneComponent;

	SkeletalMeshWeapon = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Skeletal Mesh"));
	SkeletalMeshWeapon->SetGenerateOverlapEvents(false);
	SkeletalMeshWeapon->SetCollisionProfileName(TEXT("NoCollision"));
	SkeletalMeshWeapon->SetupAttachment(RootComponent);

	StaticMeshWeapon = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Static Mesh "));
	StaticMeshWeapon->SetGenerateOverlapEvents(false);
	StaticMeshWeapon->SetCollisionProfileName(TEXT("NoCollision"));
	StaticMeshWeapon->SetupAttachment(RootComponent);

	ShootLocation = CreateDefaultSubobject<UArrowComponent>(TEXT("ShootLocation"));
	ShootLocation->SetupAttachment(RootComponent);
	ShootDropMagazine = CreateDefaultSubobject<UArrowComponent>(TEXT("ShootDropMagazine"));
	ShootDropMagazine->SetupAttachment(RootComponent);
	ShootDropSleeve = CreateDefaultSubobject<UArrowComponent>(TEXT("ShootDropSleeve"));
	ShootDropSleeve->SetupAttachment(RootComponent);

}

// Called when the game starts or when spawned
void AWeaponDefault::BeginPlay()
{
	Super::BeginPlay();

	if (IFMODStudioModule::IsAvailable())
	{
		FMOD::Studio::System* StudioSystem = IFMODStudioModule::Get().GetStudioSystem(EFMODSystemContext::Runtime);
		if (StudioSystem)
		{
			// Use it here
		}
	}

	WeaponInit();
	
}

void AWeaponDefault::WeaponInit()
{
	if (SkeletalMeshWeapon && !SkeletalMeshWeapon->SkeletalMesh)
		SkeletalMeshWeapon->DestroyComponent(true);

	if (StaticMeshWeapon && !StaticMeshWeapon->GetStaticMesh())
		StaticMeshWeapon->DestroyComponent();

//	WeaponFireType = new FWeaponFireType(new FHeadAttack());
}

void AWeaponDefault::InitWeapon(USkeletalMeshComponent* Parent, FWeaponInfo NewWeaponInfo, EMovementState State, uint8 RoundInMagazine)
{
	if (!Parent)
		return;
	
	FAttachmentTransformRules Rule(EAttachmentRule::SnapToTarget, false);
	this->AttachToComponent(Parent, Rule, FName("WeaponSocketRightHand"));

	WeaponInfo = NewWeaponInfo;
	UpdateStateWeapon(State);
	SetWeaponRound(RoundInMagazine);

	if (WeaponInfo.WeaponType == EWeaponTypes::Grenade)
	{
		SetWeaponStateFire(true);
	}
}

void AWeaponDefault::Destroyed()
{
	Super::Destroyed();
}

// Called every frame
void AWeaponDefault::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (IsEnemy && TargetActor)
		ShootEndLocation = TargetActor->GetActorLocation();

	FireTick(DeltaTime);
	DispersionTick(DeltaTime);

}

void AWeaponDefault::FireTick(float DeltaTime)
{
	
	if (WeaponFiring && !WeaponReload && FireTimer < 0.f)
	{
		UE_LOG(LogTemp, Warning, TEXT("roud: %d"), GetWeaponRound());
		if (GetWeaponRound() > 0
			|| WeaponInfo.WeaponType == EWeaponTypes::Hand
			|| WeaponInfo.WeaponType == EWeaponTypes::Grenade)
		{
			Fire();
		}

		if (GetWeaponRound() <= 0)
		{
			TryReload();
		}
	}

	FireTimer -= DeltaTime;
}

void AWeaponDefault::DispersionTick(float DeltaTime)
{
	if (!WeaponReload)
	{
		if (!WeaponFiring)
			if (ShouldReduceDispersion)
				CurrentDispersion = CurrentDispersion - CurrentDispersionReduction;
			else
				CurrentDispersion = CurrentDispersion + CurrentDispersionReduction;

		if (CurrentDispersion < CurrentDispersionMin)
			CurrentDispersion = CurrentDispersionMin;
		else
			if (CurrentDispersion > CurrentDispersionMax)
				CurrentDispersion = CurrentDispersionMax;
	}
}

FProjectileInfo AWeaponDefault::GetProjectile()
{
	return WeaponInfo.ProjectileSetting;
}

void AWeaponDefault::SpawnDynamicProjectile(UArrowComponent* Shoot, UStaticMesh* Mesh, FVector Scale, float Speed,
    float LiveTime)
{
	if (Mesh)
	{
		FTransform Transform = Shoot->GetComponentTransform();
		FVector LocalDir = Transform.GetLocation();

		AStaticMeshActor* NewActor = nullptr;


		FActorSpawnParameters Param;
		Param.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
		Param.Owner = this;
		NewActor = GetWorld()->SpawnActor<AStaticMeshActor>(AStaticMeshActor::StaticClass(), Transform, Param);
		
		if (NewActor && NewActor->GetStaticMeshComponent())
		{
			NewActor->SetActorScale3D(Scale);
			NewActor->GetStaticMeshComponent()->SetCollisionProfileName(TEXT("IgnoreOnlyPawn"));
			NewActor->GetStaticMeshComponent()->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
			
			//set parameter for new actor
			NewActor->SetActorTickEnabled(false);
			NewActor->InitialLifeSpan = LiveTime;

			NewActor->GetStaticMeshComponent()->Mobility = EComponentMobility::Movable;
			NewActor->GetStaticMeshComponent()->SetSimulatePhysics(true);
			NewActor->GetStaticMeshComponent()->SetStaticMesh(Mesh);

			NewActor->GetStaticMeshComponent()->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECollisionResponse::ECR_Ignore);
			NewActor->GetStaticMeshComponent()->SetCollisionResponseToChannel(ECC_GameTraceChannel2, ECollisionResponse::ECR_Ignore);
			NewActor->GetStaticMeshComponent()->SetCollisionResponseToChannel(ECC_Pawn, ECollisionResponse::ECR_Ignore);
			NewActor->GetStaticMeshComponent()->SetCollisionResponseToChannel(ECC_WorldStatic, ECollisionResponse::ECR_Block);
			NewActor->GetStaticMeshComponent()->SetCollisionResponseToChannel(ECC_WorldDynamic, ECollisionResponse::ECR_Block);
			NewActor->GetStaticMeshComponent()->SetCollisionResponseToChannel(ECC_PhysicsBody, ECollisionResponse::ECR_Block);
			
			NewActor->GetStaticMeshComponent()->SetMassOverrideInKg(NAME_None, 10.f, true);
			

			FVector DropImpulseDirection = Shoot->GetForwardVector();			
			if (!DropImpulseDirection.IsNearlyZero())
			{
				FVector FinalDir;
				LocalDir = LocalDir + (DropImpulseDirection * 1000.0f);

				float ImpulseRandomDispersion = 1.f;
				if (!FMath::IsNearlyZero(ImpulseRandomDispersion))
					FinalDir += UKismetMathLibrary::RandomUnitVectorInConeInDegrees(LocalDir, ImpulseRandomDispersion);
				FinalDir.GetSafeNormal(0.0001f);

				NewActor->GetStaticMeshComponent()->AddImpulse(FinalDir* Speed);
			}
		}		
	}
}

void AWeaponDefault::FireProjectile()
{	
	FVector SpawnLocation = ShootLocation->GetComponentLocation();
	FVector EndLocation = GetFireEndLocation();
	//FVector EndLocation = (ShootLocation->GetComponentLocation() - ShootEndLocation).GetSafeNormal() * -600.f + SpawnLocation;
	
	
	//todo: тут надо стрелять чуть дальше, чем указывает курсор (EndLocation)
	
	FProjectileInfo ProjectileInfo;
	ProjectileInfo = GetProjectile();
	
	if (ProjectileInfo.Projectile)
	{
		FVector Dir = EndLocation - SpawnLocation;

		Dir.Normalize();

		FMatrix myMatrix(Dir, FVector(0, 1, 0), FVector(0, 1, 1), FVector::ZeroVector);
		FRotator SpawnRotation = myMatrix.Rotator();
				
		//Projectile Init ballistic fire

		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		SpawnParams.Owner = this;
		SpawnParams.Instigator = GetInstigator();

		AWeaponProjectile* myProjectile = Cast<AWeaponProjectile>(
					GetWorld()->SpawnActor(ProjectileInfo.Projectile, &SpawnLocation, &SpawnRotation, SpawnParams));
		
		if (myProjectile)
		{													
			myProjectile->InitProjectile(WeaponInfo.ProjectileSetting);
		}

		if (WeaponInfo.EmptyBulletMesh)
		{
			SpawnDynamicProjectile(ShootDropSleeve,
             WeaponInfo.EmptyBulletMesh,
             FVector(2.0f, 2.0f, 2.0f),
             100.f,
             5.0f);
		}
	}	
}

void AWeaponDefault::FireTrace()
{
	if (WeaponInfo.LaserEffect)
	{
		UParticleSystemComponent* LaserEffect = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(),
                                                WeaponInfo.LaserEffect, FVector::ZeroVector);
		if (LaserEffect)
		{
			LaserEffect->SetBeamSourcePoint(0, ShootLocation->GetComponentLocation(), 0);
			LaserEffect->SetBeamTargetPoint(0, ShootEndLocation, 0);
		}
	}
	
	FHitResult OutHit;
	FCollisionQueryParams CollisionParams;
	CollisionParams.bReturnPhysicalMaterial = true;

	TArray<AActor*> IgnoredActors;
	IgnoredActors.Add(this);
	IgnoredActors.Add(GetOwner());

	FVector StartLocation = ShootLocation->GetComponentLocation();
	FVector EndLocation = GetActorForwardVector() * WeaponInfo.DistanceTrace + StartLocation;
						
	bool ResultLineTrace = GetWorld()->LineTraceSingleByChannel(OutHit,
        StartLocation, EndLocation,
        ECollisionChannel::ECC_Camera, CollisionParams);

#ifdef _WEAPON_DEBUG
	DrawDebugLine(GetWorld(),
            StartLocation,
            EndLocation,
            FColor::Green,
            false,
            5.f,
            (uint8)'\000',
            0.5f);
#endif	
						
	if (ResultLineTrace && OutHit.PhysMaterial.IsValid())
	{
		HitFXEvent(OutHit);	
	}

	UGameplayStatics::ApplyPointDamage(OutHit.GetActor(), WeaponInfo.WeaponDamage,
            OutHit.TraceStart, OutHit, GetInstigatorController(),this,UDamageType::StaticClass());

	UAISense_Damage::ReportDamageEvent(GetWorld(), OutHit.GetActor(), GetOwner(),
        WeaponInfo.ProjectileSetting.ProjectileDamage, ShootLocation->GetComponentLocation(), OutHit.Location);
}

// Удар ручным оружием
void AWeaponDefault::FireHand()
{

	//тут надо сделать BoxTraceSingle от лица
	SetWeaponStateFire(false);
	
	FHitResult OutHit;
	FVector Forward = ShootLocation->GetForwardVector() * 10.f;	
	FVector BoxLocation = ShootLocation->GetComponentLocation() + Forward;

	TArray<AActor*> IgnoreActors;	
	IgnoreActors.Add(this);
	IgnoreActors.Add(GetInstigator());
	
	if (GetOwner())
	{
		BoxLocation.Z = GetOwner()->GetActorLocation().Z;
		IgnoreActors.Add(GetOwner());
	}
	
	bool bResult = UKismetSystemLibrary::BoxTraceSingle(GetWorld(),
                BoxLocation, BoxLocation, FVector(40.f,40.f,60.f), FRotator::ZeroRotator, 
                UEngineTypes::ConvertToTraceType(ECollisionChannel::ECC_GameTraceChannel5),
                false, IgnoreActors,
#ifdef _WEAPON_DEBUG
                EDrawDebugTrace::Persistent,
#else
				EDrawDebugTrace::None,
#endif
                OutHit, true);

	if (bResult)
	{

		UGameplayStatics::ApplyDamage(OutHit.GetActor(), WeaponInfo.WeaponDamage, GetInstigatorController(),
         GetOwner(), NULL);
		
		UAISense_Damage::ReportDamageEvent(GetWorld(), OutHit.GetActor(), GetOwner(),
										WeaponInfo.ProjectileSetting.ProjectileDamage,
										ShootLocation->GetComponentLocation(),
										OutHit.Location);
	}

	InitWeaponReload();
}

void AWeaponDefault::FireGrenade()
{
	FProjectileInfo ProjectileInfo;
	ProjectileInfo = GetProjectile();

	if (ProjectileInfo.Projectile)
	{
		FVector ShootStartLocation = ShootLocation->GetComponentLocation();
		FVector Dir = ShootEndLocation - ShootStartLocation;
		float Distance = Dir.Size();
		
		if (Distance < 400.f)
			ProjectileInfo.ProjectileInitSpeed = Distance;
		else
			ProjectileInfo.ProjectileInitSpeed = Distance/1.5f;
		
		Dir.Normalize();

		FMatrix myMatrix(Dir, FVector(0, 1, 0), FVector(0, 1, 1), FVector::ZeroVector);
		FRotator SpawnRotation = myMatrix.Rotator();

		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		SpawnParams.Owner = GetOwner();
		SpawnParams.Instigator = GetInstigator();

		AWeaponProjectile* myProjectile = Cast<AWeaponProjectile>(
                    GetWorld()->SpawnActor(ProjectileInfo.Projectile, &ShootStartLocation, &SpawnRotation, SpawnParams));
                    
		if (myProjectile)
		{													
			myProjectile->InitProjectile(WeaponInfo.ProjectileSetting);
		}			
	}
	
	this->Destroy();
}

void AWeaponDefault::Fire()
{
	FireTimer = WeaponInfo.ShotSpeed;
	
	ChangeDispersionByShot();
	OnWeaponFireStart.Broadcast(WeaponInfo.AnimWeaponFire);

	UFMODBlueprintStatics::PlayEventAtLocation(this, WeaponInfo.StartFireSound, GetTransform(), true);
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), WeaponInfo.VFXStartFire, ShootLocation->GetComponentTransform());

	uint8 NumberProjectile = WeaponInfo.RoundProjectileByShot;

	if (ShootLocation)
	{
		//TODO: вообще это тут не должно быть. Оно должно работать из ивента анимации
		if (WeaponInfo.WeaponType == EWeaponTypes::Hand) 
		{				
			FireHand();
		}
		else if (WeaponInfo.WeaponType == EWeaponTypes::Grenade)
		{
			FireGrenade();
		}
		else
		{
			for (uint8 i = 0; i < NumberProjectile; i++)
			{
				if (WeaponInfo.WeaponType == EWeaponTypes::Projectile)
				{
					if (GetWeaponRound() > 0)
					{
						FireProjectile();						
						--Rounds;
					}
				}
				else if (WeaponInfo.WeaponType == EWeaponTypes::Trace)
				{				
					FireTrace();
				}			
			}
			
			TryReload();
			OnChangeWeaponMagazine.Broadcast(Rounds);
		}
	}	
}

FVector AWeaponDefault::GetFireEndLocation() const
{
	FVector EndLocation = FVector::ZeroVector;

	UE_LOG(LogTemp, Warning, TEXT("%s -ShootEndLocation: %f,%f,%f"), TEXT(__FUNCTION__), ShootEndLocation.X, ShootEndLocation.Y, ShootEndLocation.Z);
	FVector tmpV = (ShootLocation->GetComponentLocation() - ShootEndLocation);

	if(tmpV.Size() > SizeVectorToChangeShootDirectionLogic)
	{
		EndLocation = ShootLocation->GetComponentLocation() + ApplyDispersionToShoot(
            (ShootLocation->GetComponentLocation() - ShootEndLocation).GetSafeNormal()) * -20000.0f;

#ifdef _WEAPON_DEBUG
		DrawDebugCone(GetWorld(),
            ShootLocation->GetComponentLocation(),
            -(ShootLocation->GetComponentLocation() - ShootEndLocation),
            WeaponInfo.DistanceTrace, GetCurrentDispersion()* PI / 180.f,
            GetCurrentDispersion()* PI / 180.f, 32, FColor::Emerald,
            false, .1f, (uint8)'\000',
            1.0f);
#endif _WEAPON_DEBUG
	}
	else
	{
		EndLocation = ShootLocation->GetComponentLocation() + ApplyDispersionToShoot(ShootLocation->GetForwardVector()) * 20000.0f;

#ifdef _WEAPON_DEBUG		
		DrawDebugCone(GetWorld(),
            ShootLocation->GetComponentLocation(),
            ShootLocation->GetForwardVector(),
            WeaponInfo.DistanceTrace,
            GetCurrentDispersion()* PI / 180.f,
            GetCurrentDispersion()* PI / 180.f,
            32, FColor::Emerald, false,
            .1f, (uint8)'\000',
            1.0f);
#endif _WEAPON_DEBUG
	}

	if (EndLocation.Z < 100.f)
		EndLocation.Z = 120.f;
		

#ifdef _WEAPON_DEBUG
		//direction weapon look
		DrawDebugLine(GetWorld(),
            ShootLocation->GetComponentLocation(),
            ShootLocation->GetComponentLocation() + ShootLocation->GetForwardVector() * 500.0f,
            FColor::Cyan,
            false,
            5.f,
            (uint8)'\000',
            0.5f);
		
		//direction projectile must fly
		DrawDebugLine(GetWorld(),
            ShootLocation->GetComponentLocation(),
            ShootEndLocation,
            FColor::Red,
            false,
            5.f,
            (uint8)'\000',
            0.5f);
		
		//Direction Projectile Current fly
		DrawDebugLine(GetWorld(),
            ShootLocation->GetComponentLocation(),
            EndLocation, FColor::Black,
            false,
            5.f,
            (uint8)'\000',
            0.5f);
#endif _WEAPON_DEBUG
	
	return EndLocation;	
}

void AWeaponDefault::SetEnemyFlag(bool Flag)
{
	IsEnemy = Flag;
}

void AWeaponDefault::SetWeaponStateFire(bool bIsFire)
{
	if (!BlockFire)
	{
		WeaponFiring = bIsFire;
	}
	else
		WeaponFiring = false;
}


void AWeaponDefault::InitWeaponReload()
{
	// Непозволяет повторно запустить перезарядку
	if (WeaponReload)
		return;
	
	WeaponReload = true;
	
	//TODO: тут запускаем анимации

	// Сброс магазина, если есть
	//	if (WeaponInfo.DropMagazine)

//		SpawnDynamicProjectile(ShootDropMagazine, WeaponInfo.DropMagazine,
//							FVector(2.0f, 2.0f, 2.0f),
//							100.f,
//						5.0f);

	//todo: ReloadTime надо брать или из структуры или из времени анимации
	GetWorld()->GetTimerManager().SetTimer(ReloadTimerHandle, this,
											&AWeaponDefault::ReloadTimer, WeaponInfo.ReloadTime, false);
}

void AWeaponDefault::ReloadTimer()
{
	WeaponReload = false;

	const uint8 AviableAmmoFromInventory = GetAvailableAmmoForReload();	
	const uint8 NeedToReload = WeaponInfo.MaxRoundInMagazine - Rounds;
	uint8 TakeRounds;

	UE_LOG(LogTemp, Warning, TEXT("%s: GetAvailableAmmoForReload = %d"), TEXT(__FUNCTION__), AviableAmmoFromInventory);

	if (NeedToReload > AviableAmmoFromInventory)
		TakeRounds = AviableAmmoFromInventory;
	else
		TakeRounds = NeedToReload;

	// У моба никогда не заканчиваются патроны
	if (IsEnemy)
	{
		Rounds = WeaponInfo.MaxRoundInMagazine;
		return;
	}

	Rounds += TakeRounds;
	
	if (GetOwner())
	{
		UT_InventoryComponent* InventoryComp = Cast<UT_InventoryComponent>(GetOwner()->GetComponentByClass(UT_InventoryComponent::StaticClass()));
		if (InventoryComp)
		{
			InventoryComp->RemoveAmmo(WeaponInfo.RoundTypeName, TakeRounds);
			OnChangeWeaponMagazine.Broadcast(Rounds);
		}
	}
	
}

uint8 AWeaponDefault::GetAvailableAmmoForReload()
{
	uint8 AvailableAmmoForWeapon = WeaponInfo.MaxRoundInMagazine;
	if (GetOwner())
	{
		UT_InventoryComponent* InventoryComp = Cast<UT_InventoryComponent>(GetOwner()->GetComponentByClass(UT_InventoryComponent::StaticClass()));
		if (InventoryComp && InventoryComp->GetAmmoForWeapon(WeaponInfo.RoundTypeName, AvailableAmmoForWeapon))
		{			
			return AvailableAmmoForWeapon;
		}
	}
	return 0;
}

void AWeaponDefault::TryReload(uint8 Num)
{
	if (GetWeaponRound() <= Num && !WeaponReload)
	{
		if(CheckCanWeaponReload() || IsEnemy)
			InitWeaponReload();
	}

	OnChangeWeaponMagazine.Broadcast(Rounds);
}

bool AWeaponDefault::CheckCanWeaponReload()
{
	if (GetOwner())
	{
		UT_InventoryComponent* InventoryComp = Cast<UT_InventoryComponent>(
                                        GetOwner()->GetComponentByClass(UT_InventoryComponent::StaticClass()));
		
		if (InventoryComp && InventoryComp->CheckAmmoForWeapon(WeaponInfo.RoundTypeName))
		{
			return true;
		}
	}

	return false;
}

void AWeaponDefault::CancelReload()
{
	if (ReloadTimerHandle.IsValid())
	{		
		GetWorld()->GetTimerManager().ClearTimer(ReloadTimerHandle);
		WeaponReload = false;
	}
}

void AWeaponDefault::SetWeaponRound(int32 NumRounds)
{
	if (NumRounds > 0)
		Rounds = NumRounds;
	
	else if (!WeaponReload)
	{
		TryReload();
	}
}

void AWeaponDefault::UpdateStateWeapon(EMovementState MovementState)
{
	BlockFire = false;
	switch (MovementState)
	{
	case EMovementState::Aim_State:
		WeaponAiming = true;
		CurrentDispersionMax = WeaponInfo.DispersionWeapon.Aim_StateDispersionAimMax;
		CurrentDispersionMin = WeaponInfo.DispersionWeapon.Aim_StateDispersionAimMin;
		CurrentDispersionRecoil = WeaponInfo.DispersionWeapon.Aim_StateDispersionAimRecoil;
		CurrentDispersionReduction = WeaponInfo.DispersionWeapon.Aim_StateDispersionReduction;
		break;
	case EMovementState::Sprint_State:
		WeaponAiming = false;
		CurrentDispersionMax = WeaponInfo.DispersionWeapon.Sprint_StateDispersionAimMax;
		CurrentDispersionMin = WeaponInfo.DispersionWeapon.Sprint_StateDispersionAimMin;
		CurrentDispersionRecoil = WeaponInfo.DispersionWeapon.Sprint_StateDispersionAimRecoil;
		CurrentDispersionReduction = WeaponInfo.DispersionWeapon.Sprint_StateDispersionReduction;
		break;
	case EMovementState::Walk_State:
		WeaponAiming = false;
		CurrentDispersionMax = WeaponInfo.DispersionWeapon.Walk_StateDispersionAimMax;
		CurrentDispersionMin = WeaponInfo.DispersionWeapon.Walk_StateDispersionAimMin;
		CurrentDispersionRecoil = WeaponInfo.DispersionWeapon.Walk_StateDispersionAimRecoil;
		CurrentDispersionReduction = WeaponInfo.DispersionWeapon.Walk_StateDispersionReduction;
		break;
	case EMovementState::Crouch_State:
		WeaponAiming = true;
		CurrentDispersionMax = WeaponInfo.DispersionWeapon.Crouch_StateDispersionAimMax;
		CurrentDispersionMin = WeaponInfo.DispersionWeapon.Crouch_StateDispersionAimMin;
		CurrentDispersionRecoil = WeaponInfo.DispersionWeapon.Crouch_StateDispersionAimRecoil;
		CurrentDispersionReduction = WeaponInfo.DispersionWeapon.Crouch_StateDispersionReduction;
		break;		
	case EMovementState::Jump_State:
		WeaponAiming = false;
		BlockFire = true;
		SetWeaponStateFire(false);//set fire trigger to false
		//Block Fire
		break;
	default:
		break;
	}
}

void AWeaponDefault::HitFXEvent(FHitResult Hit)
{
	if (Hit.GetActor() && Hit.PhysMaterial.IsValid())
	{
		UPrimitiveComponent* OtherComp = Hit.GetComponent();
		EPhysicalSurface mySurfacetype = UGameplayStatics::GetSurfaceType(Hit);

		//Debug
		UE_LOG(LogTemp, Warning, TEXT("%s: surface project: %d : %s"), TEXT(__FUNCTION__), mySurfacetype, *Hit.GetActor()->GetName());
		
		if (WeaponInfo.ProjectileSetting.HitDecals.Contains(mySurfacetype))
		{
			UMaterialInterface* myMaterial = WeaponInfo.ProjectileSetting.HitDecals[mySurfacetype];
			if (myMaterial && OtherComp)
			{
				UGameplayStatics::SpawnDecalAttached(myMaterial, FVector(20.0f), OtherComp, NAME_None, Hit.ImpactPoint, Hit.ImpactNormal.Rotation(),EAttachLocation::KeepWorldPosition,10.0f);
			}
		}
		if (WeaponInfo.ProjectileSetting.HitFXs.Contains(mySurfacetype))
		{
			UParticleSystem* myParticle = WeaponInfo.ProjectileSetting.HitFXs[mySurfacetype];
			if (myParticle)
			{
				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), myParticle, FTransform(Hit.ImpactNormal.Rotation(), Hit.ImpactPoint, FVector(1.0f)));
			}
		}
			
		if (WeaponInfo.ProjectileSetting.HitSound)
		{
			UGameplayStatics::PlaySoundAtLocation(GetWorld(), WeaponInfo.ProjectileSetting.HitSound, Hit.ImpactPoint);
		}
		
		//UTypes::AddEffectBySurfaceType(Hit.GetActor(), Hit.BoneName, ProjectileSettings.Effect, mySurfacetype);
	}
}

void AWeaponDefault::ChangeDispersionByShot()
{	
	CurrentDispersion = CurrentDispersion + CurrentDispersionRecoil;
}

FVector AWeaponDefault::ApplyDispersionToShoot(FVector DirectionShoot) const
{
	return FMath::VRandCone(DirectionShoot, GetCurrentDispersion() * PI / 180.f);
}

float AWeaponDefault::GetCurrentDispersion() const
{
	return CurrentDispersion;
}



