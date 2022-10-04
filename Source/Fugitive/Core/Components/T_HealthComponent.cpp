// Fill out your copyright notice in the Description page of Project Settings.


#include "T_HealthComponent.h"


#include "Fugitive/Core/Interfaces/DisplayDamageElement.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"

// Sets default values for this component's properties
UT_HealthComponent::UT_HealthComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}

// Создаем актора с отлетающей циферкой нанесенного урона
void UT_HealthComponent::SpawnDisplayDamageElement(float Damage)
{
	if (!bShowDamangeElement || !GetOwner())
		return;
	
	UObject* DamageElementBP = Cast<UObject>(StaticLoadObject(UObject::StaticClass(), NULL,
						TEXT("Blueprint'/Game/Game/UI/Elements/Damage/BP_DisplayDamageActor.BP_DisplayDamageActor'")));
	UBlueprint* GeneratedBP = Cast<UBlueprint>(DamageElementBP);
	if (GeneratedBP)
	{
		// Подсчитываем случайное смещение от LocalPointSpawnDamage
		FVector SpawnLocation = FVector(0);
		SpawnLocation.X = UKismetMathLibrary::RandomFloatInRange(-RangeLocalPointSpawn, RangeLocalPointSpawn);
		SpawnLocation.Y = UKismetMathLibrary::RandomFloatInRange(-RangeLocalPointSpawn, RangeLocalPointSpawn);
		SpawnLocation += LocalPointSpawnDamage;

		FVector OwnerLocation = GetOwner()->GetActorLocation();
		FRotator SpawnRotation = FRotator(0);
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = GetOwner();
		SpawnLocation += OwnerLocation;
		
		AActor* SpawnedDamageElement = GetWorld()->SpawnActor(GeneratedBP->GeneratedClass, &SpawnLocation, &SpawnRotation, SpawnParams);
		if (SpawnedDamageElement
			&& UKismetSystemLibrary::DoesImplementInterface( SpawnedDamageElement, UDisplayDamageElement::StaticClass() ))
		{
			IDisplayDamageElement::Execute_SetDamageValue(SpawnedDamageElement, Damage, Health);
		}
	}
 
}


// Called when the game starts
void UT_HealthComponent::BeginPlay()
{
	Super::BeginPlay();

	if (GetOwner())
		GetOwner()->OnTakeAnyDamage.AddDynamic(this, &UT_HealthComponent::OnTakeAnyDamage);
	else
		UE_LOG(LogTemp, Error, TEXT("%s: Can't find owner"), TEXT(__FUNCTION__));
}

void UT_HealthComponent::OnTakeAnyDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType,
	 AController* InstigatedBy, AActor* DamageCauser)
{
//	UE_LOG(LogTemp, Warning, TEXT("%s: damage %f"), TEXT(__FUNCTION__), Damage);
	ApplyDamage(Damage);
}

float UT_HealthComponent::GetCurrentHealth()
{
	OnHealthChange.Broadcast(Health, Health);
	return Health;
}


void UT_HealthComponent::ApplyDamage(float Value)
{
	if (bBlockChanse && Value > 0)
	{
		int32 rnd = FMath::RandHelper(100);
		if (rnd <= BlockChanse && bBlockChanse)
		{
			// Block
			UE_LOG(LogTemp, Warning, TEXT("UT_HealthComponent::ApplyDamage - BLOCK"));
			return;
		}
	}
	else
		bBlockChanse = true;

	Health -= Value;	
	if (Health <= 0)
	{
		Health = 0.f;
		OnEmptyHealth.Broadcast(); // Отправляем сообщение, что жизней нет и мы должны умереть
	}
	else if (Health > 100)
		Health = 100.f;
		

	OnHealthChange.Broadcast(Health, Value);
	SpawnDisplayDamageElement(Value);
}

void UT_HealthComponent::Recover(float Value)
{
	ApplyDamage(-Value);
}

void UT_HealthComponent::EnableBlockChanse(bool Enable)
{
	bBlockChanse = Enable;
}


