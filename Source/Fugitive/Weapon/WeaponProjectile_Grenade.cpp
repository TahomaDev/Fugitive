// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponProjectile_Grenade.h"


#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "Perception/AISense_Damage.h"

void AWeaponProjectile_Grenade::BeginPlay()
{
	Super::BeginPlay();
	TimerEnabled = true;
	IsDynamicComponent = false;
}

void AWeaponProjectile_Grenade::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	TimerExplose(DeltaTime);
}

void AWeaponProjectile_Grenade::TimerExplose(float DeltaTime)
{
	if (TimerEnabled)
		if (TimerToExplose > TimeToExplose)
			Explose();			
		else
			TimerToExplose += DeltaTime;
}

void AWeaponProjectile_Grenade::Explose()
{
	if (IsDynamicComponent)
	{
		this->Destroy();
		return;
	}
    
	TimerEnabled = false;
	if (ProjectileSettings.ExploseFX)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ProjectileSettings.ExploseFX, GetActorLocation(), GetActorRotation(), FVector(1.0f));
	}
	if (ProjectileSettings.ExploseSound)
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), ProjectileSettings.ExploseSound, GetActorLocation());
	}


#ifdef _PROJECTILE_DEBUG
		DrawDebugSphere(GetWorld(),
            GetActorLocation(),
            ProjectileSettings.ExploseWeakRadiusDamage,
            20.f,
            FColor::Blue,
            true, 1.f);
#endif _PROJECTILE_DEBUG

	FVector Origin = GetActorLocation();
	TArray<AActor*> IgnoredActors;
	IgnoredActors.Add(this);

	UGameplayStatics::ApplyRadialDamageWithFalloff(GetWorld(),
        ProjectileSettings.ExploseMaxDamage,
        ProjectileSettings.ExploseMaxDamage*0.2f,
        Origin,
        ProjectileSettings.ExploseStrongRadiusDamage,
        ProjectileSettings.ExploseWeakRadiusDamage,
        5,
        UDamageType::StaticClass(), IgnoredActors,GetOwner()->GetParentActor(),
        nullptr, ECC_Visibility);
		
	if (IsDynamicComponent)
		return;
    
	TArray<FHitResult> OutHit;
	bool TraceResult = UKismetSystemLibrary::SphereTraceMulti(GetWorld(), Origin, Origin,
        ProjectileSettings.ExploseWeakRadiusDamage, ETraceTypeQuery::TraceTypeQuery_MAX, false,
        IgnoredActors, EDrawDebugTrace::Persistent, OutHit, true,
        FLinearColor::Red, FLinearColor::Green, 1.f );


	UE_LOG(LogTemp, Warning, TEXT("hitd: %d"), OutHit.Num());
	for(int i = 0; i < OutHit.Num(); i++)
	{
		if (OutHit[i].GetActor())
		{
			UAISense_Damage::ReportDamageEvent(GetWorld(), OutHit[i].GetActor(), GetInstigator(),
        ProjectileSettings.ProjectileDamage, OutHit[i].Location, OutHit[i].Location);
		
			if (OutHit[i].PhysMaterial.IsValid())
			{
				EPhysicalSurface mySurfacetype = UGameplayStatics::GetSurfaceType(OutHit[i]);
				UE_LOG(LogTemp, Warning, TEXT("surface grenade: %d"), mySurfacetype);
				//            UTypes::AddEffectBySurfaceType(OutHit[i].GetActor(), OutHit[i].BoneName, ProjectileSettings.Effect, mySurfacetype);
			}
		}
	}

	this->Destroy();
}

void AWeaponProjectile_Grenade::BulletCollisionSphereHit(UPrimitiveComponent* HitComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
}

void AWeaponProjectile_Grenade::ImpactProjectile()
{
	//Init Grenade
	TimerEnabled = true;
}
