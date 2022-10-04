// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "T_HealthComponent.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnHealthChange, float, Health, float, Damage);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnEmptyHealth);

UCLASS(Blueprintable, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class FUGITIVE_API UT_HealthComponent : public USceneComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UT_HealthComponent();

	// Delegates
	UPROPERTY(BlueprintAssignable, BlueprintReadWrite, Category="HealthComponent")
	FOnHealthChange OnHealthChange;
	FOnEmptyHealth OnEmptyHealth;
	
private:

	void SpawnDisplayDamageElement(float Damage);
	
	// Enable/Disable Block Chanse
	bool bBlockChanse = true;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	UFUNCTION(BlueprintCallable)
	void OnTakeAnyDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);

	// State Values
	float Health = 100.f;

public:	
	UFUNCTION(BlueprintCallable, Category="Health")
	float GetCurrentHealth(); 
	UFUNCTION(BlueprintCallable, Category="Health")
    void SetCurrentHealth(float Value) { Health = Value; }; 

	UFUNCTION(BlueprintCallable, Category="Health Change")
	void ApplyDamage(float Value);
	UFUNCTION(BlueprintCallable, Category="Health Change")
	void Recover(float Value);

	UFUNCTION(BlueprintCallable, Category="Block Chanse Enabled")
    void EnableBlockChanse(bool Enable);

	// Configs
	
	// Шанс блока при нанесении урона (макс. 100)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config|Block")
	uint8 BlockChanse = 20;

	// Создавать ли отлетающие цифры нанесенного урона
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config|Damage Element")
	bool bShowDamangeElement = true;

	// Тока вылета виджета с цифрами нанесенного урона
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config|Damage Element", Meta = (MakeEditWidget = true))
	FVector LocalPointSpawnDamage = FVector(0.f, 0.f, 100.f);

	// Диапазон +- от точки вылета виджета с цифрами нанесенного урона
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config|Damage Element")
	float RangeLocalPointSpawn = 10;
	
};

