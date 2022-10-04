// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Fugitive/Types.h"
#include "Fugitive/Core/Interfaces/IObjectReact.h"
#include "GameFramework/Character.h"
#include "BaseEnemyCharacter.generated.h"

class UT_InventoryComponent;
class UT_HealthComponent;
class UMyGameInstance;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnChangeCanMove, APawn*, ControlledPawn, bool, Value);

UCLASS()
class FUGITIVE_API ABaseEnemyCharacter : public ACharacter, public IIObjectReact
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ABaseEnemyCharacter();

	UPROPERTY(BlueprintAssignable)
	FOnChangeCanMove OnChangeCanMove;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = MouseCollision, meta = (AllowPrivateAccess = "true"))
	class UCapsuleComponent* MouseCollision = nullptr;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = HealthComponent, meta = (AllowPrivateAccess = "true"))
	class UT_HealthComponent* HealthComponent = nullptr;;

	void Destroyed() override;
	void OnDropLoot();
	void OnDead();

	UMyGameInstance* GameInstance;
	
	/* Жизни, выпалающий лут */
	FEnemyInfoTable EnemyInfo;

	void SpawnRandomLoot(FDropLootInfo* LootInfo);
	/** @Tahoma. Название оружия, которое ранее было помещено в руку. Служит для отмены смены оружия на такое же */
	FName CurrentWeaponName = NAME_None;
	
	/** @Tahoma. Делегат. Проигрывание анимации персонажа при стрельбе
	*/
	UFUNCTION()
    void OnFire(UAnimSequence* Anim);
	/** @Tahoma. Прекращаем стрельбу при переходе в другое состояние AI */
	UFUNCTION(BlueprintCallable)
	void SetWeaponStateFire(bool bState);
	
	/** @Tahoma. Dead */
	UFUNCTION()
    void OnEmptyHealth();
	
	FTimerHandle RagdollTimerHandle;
	FTimerHandle EnableThrowGrenadeTimerHandle;
	
	UFUNCTION()
    void EnableRagdoll();
	UFUNCTION()
	void EnableThrowGrenade();

	/** @Tahoma. Предыдущее значение скорости моба.
	 * Используется при setcanmove
	**/
	FVector OldVelocity = FVector(0.f);

	// Flags
	bool bCanMove = true;
	
public:

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void OnEmptyHealtBP();

	UFUNCTION(BlueprintCallable)
    void WeaponFire(AActor* EnemyActor);
	UFUNCTION(BlueprintCallable)
    void GrenadeFire(AActor* EnemyActor);

	/** @Tahoma. Текущее выбранное оружие
	**/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Config|Info")
	FString EnemyTableName;

	/** @Tahoma. Текущее выбранное оружие
	**/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Config|Weapon")
	uint8 CurrentWeaponSlot = 0;
	
	/** @Tahoma. Минимальное расстояние, с которого моб может стрелять
	**/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Config|Weapon|Fire")
	float FireDistanceMin = 300.f;
	
	/** @Tahoma. Максимальное расстояние, с которого моб может стрелять
	 **/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Config|Weapon|Fire")
	float FireDistanceMax = 600.f;

	/** @Tahoma. Минимальное расстояние, с которого возможно бросить гранату
	**/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Config|Weapon|Grenade")
	float ThrowGrenadeMinDistance = 400.f;
	
	/** @Tahoma. Максимальное расстояние, с которого возможно бросить гранату
	**/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Config|Weapon|Grenade")
	float ThrowGrenadeMaxDistance = 800.f;

	/** @Tahoma. Шанс броска гранаты (0..1)
	**/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Config|Weapon|Grenade")
	float ThrowGrenadeChanse = 0.2f;
	
	/** @Tahoma. В течении какого времени нельзя снова бросить гранату (сек)
	**/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Config|Weapon|Grenade")
	float ThrowGrenadeReloadSec = 15.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Config|Weapon")
	TArray<FItemInfo> WeaponSlots = {FItemInfo("Billie_v1", EItemType::WeaponItem, 1),
									FItemInfo("Shotgun_v2", EItemType::WeaponItem, 1),
									FItemInfo("Grenade_v1", EItemType::WeaponItem, 10)};

	void CharacterInitWeapon(FName IdWeaponName);
	AWeaponDefault* CurrentWeapon = nullptr;
	AWeaponDefault* CurrentWeaponGrenade = nullptr;

	/** @Tahoma. Смена оружия по выбранному слоту, где
	 * 0 - ручное оружие
	 * 1 - огнестрельное оружие
	 * 2 - гранаты
	 */
	UFUNCTION(BlueprintCallable)
	void TakeSelectWeapon(uint8 WeaponSlot);


	/** @Tahoma. Сообщаем оружию, что надо уже прекратить стрелять
	 */
	UFUNCTION(BlueprintCallable)
	void StopFire();

	UFUNCTION(BlueprintCallable)
	bool CanFire();
	UFUNCTION(BlueprintCallable)
    bool CanFireAttackPawn(FVector EnemyLocation);
	UFUNCTION(BlueprintCallable)
	bool CanThrowGrenade(AActor* TargetActor);

	// Move
	UFUNCTION(BlueprintCallable)
	bool GetCanMove() { return bCanMove;};
	UFUNCTION(BlueprintCallable)
    void SetCanMove(bool CanMove);
	
	// Flags	
	UPROPERTY(BlueprintReadWrite, Category="Attack")
	bool bCanAttackPawn = true;
	UPROPERTY(BlueprintReadWrite, Category="Attack")
	bool bCanThrowGrenade = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animations")
	TArray<UAnimMontage*> DeadsAnim;

	
};

