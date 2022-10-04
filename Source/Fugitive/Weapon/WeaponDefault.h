// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "FMODBlueprintStatics.h"
#include "Fugitive/Types.h"
#include "GameFramework/Actor.h"
#include "WeaponDefault.generated.h"

//#define _WEAPON_DEBUG

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWeaponFireStart, UAnimSequence*,Anim);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnChangeWeaponMagazine, uint8, Round);

UCLASS()
class FUGITIVE_API AWeaponDefault : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWeaponDefault();

	FOnWeaponFireStart OnWeaponFireStart;	
	FOnChangeWeaponMagazine OnChangeWeaponMagazine;

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Weapon, meta = (AllowPrivateAccess = "true"))
	class USceneComponent* SceneComponent = nullptr;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Weapon, meta = (AllowPrivateAccess = "true"))
	class USkeletalMeshComponent* SkeletalMeshWeapon = nullptr;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Weapon, meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* StaticMeshWeapon = nullptr;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Weapon, meta = (AllowPrivateAccess = "true"))
	class UArrowComponent* ShootLocation = nullptr;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Weapon, meta = (AllowPrivateAccess = "true"))
	class UArrowComponent* ShootDropMagazine = nullptr;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Weapon, meta = (AllowPrivateAccess = "true"))
	class UArrowComponent* ShootDropSleeve = nullptr;

private:
	void WeaponInit();
	void Destroyed() override;

	FFMODEventInstance InstanceWrapper;
	
	//Dispersion
	void ChangeDispersionByShot();
	FVector ApplyDispersionToShoot(FVector DirectionShoot) const;
	float GetCurrentDispersion() const;
	void DispersionTick(float DeltaTime);
	
	bool ShouldReduceDispersion = false;
	float CurrentDispersion = 0.0f;
	float CurrentDispersionMax = 1.0f;
	float CurrentDispersionMin = 0.1f;
	float CurrentDispersionRecoil = 0.1f;
	float CurrentDispersionReduction = 0.1f;

	// Projectile
	FProjectileInfo GetProjectile();
	void SpawnDynamicProjectile(UArrowComponent* Shoot, UStaticMesh* Mesh, FVector Scale, float Speed, float LiveTime = 5.0f);
	
	int32 Rounds = 0;

	// Fire
	void FireTick(float DeltaTime);
	void Fire();	
	FVector GetFireEndLocation() const;
	void FireProjectile();
	void FireTrace();
	void FireHand();
	void FireGrenade();

	// Reload
	bool CheckCanWeaponReload();
	void InitWeaponReload();
	void ReloadTimer();
	uint8 GetAvailableAmmoForReload();
	FTimerHandle ReloadTimerHandle;
	
	
	float FireTimer = 0.0f;
	float SizeVectorToChangeShootDirectionLogic = 100.0f;

	// Flags
	bool WeaponFiring = false;
	bool BlockFire = false;
	bool WeaponAiming = false;
	bool WeaponReload = false;
	bool IsEnemy = false;

public:	

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FWeaponInfo WeaponInfo; // WeaponSettings

	void SetEnemyFlag(bool Flag);
	int32 GetWeaponRound() { return Rounds; };
	void SetWeaponRound(int32 NumRounds);
	void UpdateStateWeapon(EMovementState MovementState);
	void HitFXEvent(FHitResult Hit);
	
	void TryReload(uint8 Num = 0);
	bool IsReload() { return WeaponReload; }
	void CancelReload();

	FVector ShootEndLocation = FVector(0);

	//Enemy
	AActor* TargetActor = nullptr;

	UFUNCTION(BlueprintCallable)
	void SetWeaponStateFire(bool bIsFire);


	void InitWeapon(USkeletalMeshComponent* Parent, FWeaponInfo NewWeaponInfo, EMovementState State, uint8 RoundInMagazine);
};
