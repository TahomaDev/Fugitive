// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "Components/SphereComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"

#include "Fugitive/Types.h"
#include "WeaponProjectile.generated.h"

//#define _PROJECTILE_DEBUG

UCLASS()
class FUGITIVE_API AWeaponProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWeaponProjectile();

	USphereComponent* BulletCollisionSphere = nullptr;
	UStaticMeshComponent* BulletMesh = nullptr;	
	UParticleSystemComponent* BulletFX = nullptr;
	UProjectileMovementComponent* BulletProjectileMovement;
	FProjectileInfo ProjectileSettings;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	FVector MyStartLocation = FVector::ZeroVector;

public:	

	void InitProjectile(FProjectileInfo InitParam);
	
	UFUNCTION()
    virtual void BulletCollisionSphereHit(class UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
	UFUNCTION()
    void BulletCollisionSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
    void BulletCollisionSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);


	UFUNCTION()
    virtual void ImpactProjectile();
	bool IsDynamicComponent = false;

};
