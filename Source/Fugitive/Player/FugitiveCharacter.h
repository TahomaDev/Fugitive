// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "GameFramework/Character.h"

//#include "Fugitive/Core/Components/T_InventoryComponent.h"
//#include "Fugitive/Core/Components/T_HealthComponent.h"
#include "FugitivePlayerController.h"

#include "Fugitive/Types.h"
#include "FugitiveCharacter.generated.h"

// Settings ///////////////////////////////////////////////////////////////////////////////////////////////////////////
//#define JUMP_ENABLE
#define CROUCH_ENABLE
#define CAMERAMOVEAWAY_ENABLE
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class AWeaponDefault;
class UT_InventoryComponent;
class UT_HealthComponent;

UCLASS(Blueprintable)
class AFugitiveCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AFugitiveCharacter();

	// Called every frame.
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* NewInputComponent) override;
	virtual void OnMovementModeChanged(EMovementMode PreviousMovementMode, uint8 PreviousCustomMode) override;
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	/** Returns TopDownCameraComponent subobject **/
	FORCEINLINE class UCameraComponent* GetTopDownCameraComponent() const { return TopDownCameraComponent; }
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns CursorToWorld subobject **/
	FORCEINLINE class UDecalComponent* GetCursorToWorld() { return CursorToWorld; }

protected:
	void InputActionPressed(float Value);
	void InputAttackPressed();
	void InputAttackReleased();
	void InputSelectHandWeapon1() { InputSelectHandWeapon(0); };
	void InputSelectHandWeapon2() { InputSelectHandWeapon(1); };

#ifdef JUMP_ENABLE
	void InputJumpPressed();
#endif

	void InputRunPressed();
	void InputRunReleased();

#ifdef CROUCH_ENABLE	
	void InputCrouchPressed();
	void InputCrouchReleased();
#endif CROUCH_ENABLE	

#ifdef CAMERAMOVEAWAY_ENABLE
	void InputCameraAxis(float Value);
#endif CAMERAMOVEAWAY_ENABLE	
	

private:
	/** Top down camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* TopDownCameraComponent;

	/** Camera boom positioning the camera above the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** A decal that projects to the cursor location. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UDecalComponent* CursorToWorld;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Inventory, meta = (AllowPrivateAccess = "true"))
	class UT_InventoryComponent* InventoryComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Inventory, meta = (AllowPrivateAccess = "true"))
    class UT_HealthComponent* HealthComponent;

	void InputSelectHandWeapon(uint8 InventorySlot);

	void CheckMoveToActorFinish();
	void MovementSpeedUpdate();
	void MoveToInteractActor(AActor* TargetActor, float Distance);
	void MovementTick(float DeltaSeconds);


	// Variables
	FCharacterSpeed				CharacterSpeed;
	/** @Tahoma. Текущие состояние персонажа */
	EMovementState				MovementState = EMovementState::Walk_State;
	/** @Tahoma. Последние вычисленные координаты мира под курсором */
	FVector						CursorLocation;
	/** @Tahoma. Хэндл таймера для паузы на проигрывание анимации смерти */
	FTimerHandle				RagdollTimerHandle;
	/** @Tahoma. Последние данные полученные от трассировки курсора на экране */
	FHitResult					MouseClickHitResult;
	/** @Tahoma. Указатель на актор, к которому идет перс в данный момент */
	AActor* MyTargetInteractActor = nullptr;
	
	UPROPERTY(VisibleAnywhere, Category = "Player Controller", meta = (AllowPrivateAccess = "true"))
	AFugitivePlayerController*	MyPlayerController;

	// Move the camera after using the mouse wheel
#ifdef CAMERAMOVEAWAY_ENABLE
	void CameraMoveAway(float DeltaSeconds);
	bool bIsCameraMovesAway = false;
	float MoveToCameraDistance;
#endif CAMERAMOVEAWAY_ENABLE	

	// Dead
	UFUNCTION()
	void OnEmptyHealth();
	void OnDead();
	UFUNCTION()
    void EnableRagdoll();
	// --Dead
	
	// Weapon (private) ///////////////////////////////////////////////////////
	void InputWeaponReload();
	UFUNCTION(BlueprintCallable)
	void OnChangeWeaponMagazine(uint8 Rounds);
	AWeaponDefault* CurrentWeapon = nullptr;
	uint8 SelectedWeaponHeadSlot = -1;

	// Flags //////////////////////////////////////////////////////////////////
	bool bIsAlive = true;
	bool bIsFiredNow = false;
	bool bIsMovedToInteractionActor = false;

public:

	UFUNCTION(BlueprintCallable, Category="Move")
    FVector2D GetPawnMoveDirection();
	
//#ifdef CAMERAMOVEAWAY_ENABLE	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config|Camera")
	float MinCameraDistance = 400.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config|Camera")
	float MaxCameraDistance = 800.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config|Camera")
	float MoveCameraDistance = 100.f;
//#endif CAMERAMOVEAWAY_ENABLE

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config|Move|FootstepHearing")
	float WalkHearingRange = 400.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config|Move|FootstepHearing")
	float RunHearingRange = 600.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config|Move|Interactive")
	float MinDistanceToInteractActor = 135.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config|Move|Interactive")
	float MaxNavPathDistance = 1000.f;

//	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config|Use", meta = (AllowPrivateAccess = "true"))	
//	float PickupDistance = 120.f;

	UFUNCTION(BlueprintCallable)
	void CheckMoveGatheringCancel();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void OnGathering(FItemInfo ItemInfo, AActor* TargetActor);

	UFUNCTION(BlueprintCallable)
	UT_InventoryComponent* GetInventory();

	/** @Tahoma. Функция вызываемая при ивенте перемещения персонажа */
	UFUNCTION(BlueprintCallable)
	void OnFootStep();


	UFUNCTION(BlueprintCallable)
	void HealthRecover(float Value);
	UFUNCTION(BlueprintCallable)
	bool IsAlive() { return bIsAlive; };

// Weapon
	UFUNCTION(BlueprintCallable)
	void CharacterInitWeapon(FName IdWeaponName, uint8 RoundInMagazine);
	UFUNCTION(BlueprintCallable)
    AWeaponDefault* GetCurrentWeapon() { return CurrentWeapon; };
	UFUNCTION(BlueprintCallable)
	void AttackCharEvent(bool bIsFiring);
	UFUNCTION(BlueprintCallable)
    void OnStopGathering();

	void BreakWeaponHead(uint8 InventorySlot);
	uint8 GetWeaponRound();

	UPROPERTY(BlueprintReadWrite)
	AActor* GatheringActor = nullptr;
	UPROPERTY(BlueprintReadWrite)
	FItemInfo GatheringInfo;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animations")
	TArray<UAnimMontage*> DeadsAnim;


	UFUNCTION(BlueprintNativeEvent)
	void OnHandAttack();
	

	// Flags
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bCanAttack = true;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsGathering = false;

public:

	UT_HealthComponent* GetHealthComponent() const {return HealthComponent;};



	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void OnGatheringCPP(FItemInfo ItemInfo, AActor* TargetActor);
};


