// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "GameFramework/PlayerController.h"
#include "FugitivePlayerController.generated.h"

UCLASS()
class AFugitivePlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	AFugitivePlayerController();

	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable)
	void SetCanMove(bool Value) { CanMove = Value; }
	UFUNCTION(BlueprintCallable)
	bool GetCanMove() { return CanMove; }

private:
	// Functions
//	void CursorHittedToItem(AActor* HitActor);
//	bool CanApplyItem();
//	void ApplyItem();
//	void PickupItem(UT_InventoryComponent* InventoryComponent, FName ItemName, EInventoryItemType ItemType);
//	void UseItem(FName ItemName);
	
	

	// Variables
	bool			CanMove = true;
	AActor*			SelectableActor = nullptr;
	APawn*			MyPawn = nullptr;

	// Flags

protected:
	/** True if the controlled character should navigate to the mouse cursor. */
	uint32 bMoveToMouseCursor : 1;

	// Begin PlayerController interface
	virtual void PlayerTick(float DeltaTime) override;
	virtual void SetupInputComponent() override;
	// End PlayerController interface

	/** Navigate player to the current touch location. */
	void MoveToTouchLocation(const ETouchIndex::Type FingerIndex, const FVector Location);
	
	/** Input handlers for SetDestination action. */
	void OnSetDestinationPressed();
	void OnSetDestinationReleased();

	/** Move */
	void MoveToAxisX(float value);
	void MoveToAxisY(float value);
	void MoveToAxis();
	float AxisX = 0.f;
	float AxisY = 0.f;	

public:
	// Functions
	UFUNCTION(BlueprintNativeEvent)
	void OnInteractActionStart();
	
	UFUNCTION(BlueprintCallable, Category="Move")
	FVector2D GetInputMoveDirection();
	
	UFUNCTION(BlueprintNativeEvent)
	void OnCancelItemReact(float Distance);

	UFUNCTION(BlueprintCallable, Category="Inventory")
	bool OnSwapInventoryItems(int ItemIndex1, int ItemIndex2);

	UFUNCTION(BlueprintCallable, Category="Inventory")
	bool CheckForObstacles(AActor* Target);

	/** @Tahoma. Создание и отрисовка основного игрового виджета */
	UFUNCTION(Client, Unreliable, BlueprintCallable, Category="Widget")
    void OnCreateMainGameWidget();

	/** @Tahoma. Класс главного игрового виджета
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Config|Widgets")
	TSubclassOf<UUserWidget> MainGamePlayWidget = nullptr;

	/** @Tahoma. Ссылка на созданный игровой виджет (только чтение из БП)
	*/
	UPROPERTY(BlueprintReadOnly)
	UUserWidget* MainGamePlayWidget_Ref = nullptr;

	// Variables
	UFUNCTION(BlueprintCallable)
	APawn* GetMyPawn();
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	int MaxPickupItemsAtTime = 10;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	FVector ItemLocation = FVector::ZeroVector;

	/** @Tahoma. Возвращает актора находящегося под курсором в текущий момент */
	UFUNCTION(BlueprintCallable)
	AActor* GetActorUnderCursor () const { return SelectableActor; }

	/** Navigate player to the given world location. */
	void SetNewMoveDestination(const FVector DestLocation);

	float GetNavigationPathLength(FVector ActorLocation, FVector TargetLocation);

	//Flags

};


