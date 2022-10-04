// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "WeaponProjectile.h"
#include "WeaponProjectile_Grenade.generated.h"

/**
 * 
 */
UCLASS()
class FUGITIVE_API AWeaponProjectile_Grenade : public AWeaponProjectile
{
	GENERATED_BODY()

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

public:
	void TimerExplose(float DeltaTime);
	void Explose();
	
	virtual void BulletCollisionSphereHit(class UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit) override;	
	virtual void ImpactProjectile() override;	

	bool TimerEnabled = false;
	float TimerToExplose = 0.0f;
	float TimeToExplose = 5.0f;
};
