// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponProjectile.h"

#include "WeaponDefault.h"
#include "Particles/ParticleSystemComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Perception/AISense_Damage.h"

// Sets default values
AWeaponProjectile::AWeaponProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	BulletCollisionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("Collision Sphere"));
	BulletCollisionSphere->SetSphereRadius(16.f);
	BulletCollisionSphere->bReturnMaterialOnMove = true;//hit event return physMaterial
	BulletCollisionSphere->SetCanEverAffectNavigation(false);
	BulletCollisionSphere->SetCollisionObjectType(ECC_PhysicsBody);
	BulletCollisionSphere->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	BulletCollisionSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
	
	BulletCollisionSphere->SetGenerateOverlapEvents(true);
	RootComponent = BulletCollisionSphere;
	
	BulletMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Bullet Projectile Mesh"));	
	BulletMesh->SetupAttachment(RootComponent);
	BulletMesh->SetCanEverAffectNavigation(false);
	BulletMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	BulletMesh->SetWorldRotation(FRotator(90.0f, 00.f, 0));	

	BulletFX = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("Bullet FX"));
	BulletFX->SetWorldRotation(FRotator(0.0f, 90.f, 0));
	BulletFX->SetupAttachment(RootComponent);

	//BulletSound = CreateDefaultSubobject<UAudioComponent>(TEXT("Bullet Audio"));
	//BulletSound->SetupAttachment(RootComponent);

	BulletProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("Bullet ProjectileMovement"));

	BulletProjectileMovement->InitialSpeed = 1.0f;
	BulletProjectileMovement->MaxSpeed = 100.f;

	BulletProjectileMovement->bRotationFollowsVelocity = true;
	BulletProjectileMovement->bShouldBounce = true;
	BulletProjectileMovement->ProjectileGravityScale = 0.1f;
	BulletProjectileMovement->UpdatedComponent = RootComponent;

}

// Called when the game starts or when spawned
void AWeaponProjectile::BeginPlay()
{
	Super::BeginPlay();

	BulletCollisionSphere->OnComponentHit.AddDynamic(this, &AWeaponProjectile::BulletCollisionSphereHit);
	BulletCollisionSphere->OnComponentBeginOverlap.AddDynamic(this, &AWeaponProjectile::BulletCollisionSphereBeginOverlap);
	BulletCollisionSphere->OnComponentEndOverlap.AddDynamic(this, &AWeaponProjectile::BulletCollisionSphereEndOverlap);

	MyStartLocation = GetActorLocation();
	
}

void AWeaponProjectile::InitProjectile(FProjectileInfo InitParam)
{
	
	if (InitParam.BulletMesh)
	{		
		BulletMesh->SetStaticMesh(InitParam.BulletMesh);
		BulletMesh->SetWorldScale3D(InitParam.BulletScale);
	}
	if (InitParam.BulletFX)
	{
		BulletFX->SetTemplate(InitParam.BulletFX);
	} else
	{
		BulletFX->Deactivate();
		BulletFX->DestroyComponent();
	}

	BulletProjectileMovement->ProjectileGravityScale = InitParam.ProjectileGravityScale;
	BulletProjectileMovement->bShouldBounce = InitParam.ShouldBounce;
	
	BulletProjectileMovement->MaxSpeed = InitParam.ProjectileInitSpeed;
	BulletProjectileMovement->Velocity *= InitParam.ProjectileInitSpeed;
	BulletProjectileMovement->UpdateComponentVelocity();

	this->SetLifeSpan(InitParam.ProjectileLifeTime);
	ProjectileSettings = InitParam;
}

void AWeaponProjectile::BulletCollisionSphereHit(UPrimitiveComponent* HitComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (IsDynamicComponent)
		return;

	AWeaponDefault* Weapon = Cast<AWeaponDefault>(GetOwner());
	if (Weapon != nullptr)
	{
		Weapon->HitFXEvent(Hit);
	}
	
	// Проверка, чтобы не стрелять в самого себя
	bool bItsMe = false;

	
	AActor* MyActor = GetOwner();
	if (MyActor == OtherActor)
		bItsMe = true;
	else if (MyActor)
	{
		MyActor = GetOwner()->GetOwner();
		if (MyActor == OtherActor)
			bItsMe = true;
		else if (MyActor)
		{
			MyActor = GetOwner()->GetOwner()->GetParentActor();
			if (MyActor == OtherActor)
				bItsMe = true;
		}
	}
	
	if (!bItsMe)
	{
		UGameplayStatics::ApplyDamage(OtherActor, ProjectileSettings.ProjectileDamage, GetInstigatorController(),
         this, NULL);
		UAISense_Damage::ReportDamageEvent(GetWorld(), OtherActor, GetInstigator(),
       ProjectileSettings.ProjectileDamage, MyStartLocation, Hit.Location);
	}
		
	ImpactProjectile();
}

void AWeaponProjectile::BulletCollisionSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
}

void AWeaponProjectile::BulletCollisionSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
}

void AWeaponProjectile::ImpactProjectile()
{
	this->Destroy();
}

