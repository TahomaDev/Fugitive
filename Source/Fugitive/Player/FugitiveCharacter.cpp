// Copyright Epic Games, Inc. All Rights Reserved.

#include "FugitiveCharacter.h"

#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "Camera/CameraComponent.h"
#include "Components/DecalComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/SpringArmComponent.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "Fugitive/Weapon/WeaponDefault.h"
#include "Fugitive/Core/MyGameInstance.h"
#include "Fugitive/Core/Components/T_HealthComponent.h"
#include "Fugitive/Core/Components/T_InventoryComponent.h"
#include "Fugitive/Core/Interfaces/IObjectReact.h"
#include "Kismet/KismetMathLibrary.h"
#include "Perception/AISense_Hearing.h"

AFugitiveCharacter::AFugitiveCharacter()
{
	// Set size for player capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// Don't rotate character to camera direction
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Rotate character to moving direction
	GetCharacterMovement()->RotationRate = FRotator(0.f, 640.f, 0.f);
	GetCharacterMovement()->bConstrainToPlane = true;
	GetCharacterMovement()->bSnapToPlaneAtStart = true;

#ifdef CROUCH_ENABLE	
	GetCharacterMovement()->NavAgentProps.bCanCrouch = true;
#else
	GetCharacterMovement()->NavAgentProps.bCanCrouch = false;
#endif CROUCH_ENABLE	

	// Create a camera boom...
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->SetUsingAbsoluteRotation(true); // Don't want arm to rotate when character does
	CameraBoom->TargetArmLength = 800.f;
	CameraBoom->SetRelativeRotation(FRotator(-60.f, 0.f, 0.f));
	CameraBoom->bDoCollisionTest = false; // Don't want to pull camera in when it collides with level

	// Create a camera...
	TopDownCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("TopDownCamera"));
	TopDownCameraComponent->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	TopDownCameraComponent->bUsePawnControlRotation = false; // Camera does not rotate relative to arm
	
	InventoryComponent = CreateDefaultSubobject<UT_InventoryComponent>(TEXT("InventoryComponent"));
	HealthComponent = CreateDefaultSubobject<UT_HealthComponent>(TEXT("HealthComponent"));


	// Activate ticking in order to update the cursor every frame.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;

	this->Tags.Add(FName("Player"));

	AddMovementInput(FVector(1.f, 0, 0), 0);

	APlayerController* MyPC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (MyPC)
	{
		MyPlayerController = Cast<AFugitivePlayerController>(MyPC);
	}
			
}

void AFugitiveCharacter::BeginPlay()
{
	Super::BeginPlay();

	MovementSpeedUpdate();
	if (HealthComponent)
	{		
		HealthComponent->GetCurrentHealth();
		HealthComponent->OnEmptyHealth.AddDynamic(this, &AFugitiveCharacter::OnEmptyHealth);
	}
}


void AFugitiveCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	// Если мертвы, то никаких действий не производим
	if (!bIsAlive)
		return;
		
#ifdef CAMERAMOVEAWAY_ENABLE
	// Move the camera after using the mouse wheel	
	if (bIsCameraMovesAway)
		CameraMoveAway(DeltaSeconds);
#endif CAMERAMOVEAWAY_ENABLE

	MovementTick(DeltaSeconds);

	if (bIsGathering)
		CheckMoveGatheringCancel();
}

void AFugitiveCharacter::MovementTick(float DeltaSeconds)
{
	if (!MyPlayerController)
		return;

	
	if (MyPlayerController->GetCanMove()
		&& MyPlayerController->GetHitResultUnderCursor(ECC_Visibility, true, MouseClickHitResult))
	{
		CursorLocation = MouseClickHitResult.Location;
			
		float Yaw = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), MouseClickHitResult.Location).Yaw;
		SetActorRotation(FQuat(FRotator(0.f, Yaw, 0.f)));	

		if (CurrentWeapon)
		{
			FVector Displacement = FVector(0);
			CurrentWeapon->ShootEndLocation = MouseClickHitResult.Location + Displacement;
		}
	}

	if (bIsMovedToInteractionActor)
		CheckMoveToActorFinish();
}

void AFugitiveCharacter::SetupPlayerInputComponent(UInputComponent* NewInputComponent)
{
	Super::SetupPlayerInputComponent(NewInputComponent);

#ifdef CAMERAMOVEAWAY_ENABLE
	NewInputComponent->BindAxis(TEXT("TurnCamera"), this, &AFugitiveCharacter::InputCameraAxis);
#endif CAMERAMOVEAWAY_ENABLE	

	NewInputComponent->BindAxis(TEXT("MouseMovement"), this, &AFugitiveCharacter::InputActionPressed);

	NewInputComponent->BindAction(TEXT("SelectHandWeapon1"), EInputEvent::IE_Released, this, &AFugitiveCharacter::InputSelectHandWeapon1);
	NewInputComponent->BindAction(TEXT("SelectHandWeapon2"), EInputEvent::IE_Released, this, &AFugitiveCharacter::InputSelectHandWeapon2);
	NewInputComponent->BindAction(TEXT("WeaponReload"), EInputEvent::IE_Released, this, &AFugitiveCharacter::InputWeaponReload);

#ifdef JUMP_ENABLE	
	NewInputComponent->BindAction(TEXT("JumpEvent"), EInputEvent::IE_Pressed, this, &AFugitiveCharacter::InputJumpPressed);
#endif JUMP_ENABLE

#ifdef CROUCH_ENABLE	
	NewInputComponent->BindAction(TEXT("CrouchEvent"), EInputEvent::IE_Pressed, this, &AFugitiveCharacter::InputCrouchPressed);
	NewInputComponent->BindAction(TEXT("CrouchEvent"), EInputEvent::IE_Released, this, &AFugitiveCharacter::InputCrouchReleased);
#endif CROUCH_ENABLE	

	NewInputComponent->BindAction(TEXT("RunEvent"), EInputEvent::IE_Pressed, this, &AFugitiveCharacter::InputRunPressed);
	NewInputComponent->BindAction(TEXT("RunEvent"), EInputEvent::IE_Released, this, &AFugitiveCharacter::InputRunReleased);		
}

void AFugitiveCharacter::OnMovementModeChanged(EMovementMode PreviousMovementMode, uint8 PreviousCustomMode)
{
	Super::OnMovementModeChanged(PreviousMovementMode, PreviousCustomMode);

#ifdef JUMP_ENABLE	
	if (PreviousMovementMode == EMovementMode::MOVE_Falling
		&& GetCharacterMovement()->MovementMode == EMovementMode::MOVE_Walking)
	{
		StopJumping();
		MovementState = EMovementState::Walk_State;
		MovementSpeedUpdate();
		UE_LOG(LogTemp, Warning, TEXT("%s - %d:%d"), TEXT(__FUNCTION__), GetCharacterMovement()->MovementMode, PreviousMovementMode);
	}
#endif	
			
}

float AFugitiveCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator,
	AActor* DamageCauser)
{
	OnStopGathering();
	return Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
}

void AFugitiveCharacter::InputActionPressed(float Value)
{
	if (MyPlayerController && Value != 0.f)
	{
		bool bIsActionFind = false;		
		float Distance = MyPlayerController->GetNavigationPathLength(GetActorLocation(), MouseClickHitResult.ImpactPoint);
		
		if (Distance <= MaxNavPathDistance) // Не будет ничего делать, если идти ему далеко 
		{
			AActor* TargetActor = MouseClickHitResult.GetActor();
			if (TargetActor)
			{				
				// Курсор на что-то наткнулся. Определяем на что и работаем с этими данными
				if (TargetActor->GetClass()->ImplementsInterface(UIObjectReact::StaticClass()))
				{
					EObjectAction ObjectAction = IIObjectReact::Execute_HowAreYou(MouseClickHitResult.GetActor(), this);
					switch(ObjectAction)
					{
					case EObjectAction::Pickup:						
						UE_LOG(LogTemp, Warning, TEXT("%s - EObjectAction::Pickup"), TEXT(__FUNCTION__));
						MoveToInteractActor(TargetActor, Distance);
						break;
					case EObjectAction::Interaction:
						UE_LOG(LogTemp, Warning, TEXT("%s - EObjectAction::Interaction"), TEXT(__FUNCTION__));
						MoveToInteractActor(TargetActor, Distance);
						break;					
					case EObjectAction::InteractNPC:
						UE_LOG(LogTemp, Warning, TEXT("%s - EObjectAction::InteractNPC"), TEXT(__FUNCTION__));
						MoveToInteractActor(TargetActor, Distance);
						break;
					case EObjectAction::Enemy:
						UE_LOG(LogTemp, Warning, TEXT("%s - EObjectAction::Enemy"), TEXT(__FUNCTION__));
						//todo: тут нужно переходить в фазу боя
						InputAttackPressed();
						break;
					case EObjectAction::None:
					default:
						UE_LOG(LogTemp, Warning, TEXT("%s - EObjectAction::None"), TEXT(__FUNCTION__));
						;
					}
					
				}				
			}
			if (!bIsActionFind && MouseClickHitResult.bBlockingHit)
			{			
				MyPlayerController->SetNewMoveDestination(MouseClickHitResult.ImpactPoint);
			}
		}
	}
	else
		InputAttackReleased();
	
}

void AFugitiveCharacter::InputAttackPressed()
{
	bIsFiredNow = true;
	if (CurrentWeapon && bCanAttack)
	{
		if (CurrentWeapon->WeaponInfo.WeaponType == EWeaponTypes::Hand)
		{
			if (!CurrentWeapon->IsReload())
			{				
				OnHandAttack();
				bCanAttack = false;
			}
		}
		else
		{
			AttackCharEvent(true);
		}
	}
}

void AFugitiveCharacter::InputAttackReleased()
{
	if (bIsFiredNow)
	{
		bIsFiredNow = false;
		AttackCharEvent(false);		
	}
}

void AFugitiveCharacter::AttackCharEvent(bool bIsFiring)
{
	if (CurrentWeapon)
	{
		CurrentWeapon->SetWeaponStateFire(bIsFiring);
	}
	else
		UE_LOG(LogTemp, Warning, TEXT("%s - CurrentWeapon (NULL)"), TEXT(__FUNCTION__));
}

#ifdef JUMP_ENABLE
void AFugitiveCharacter::InputJumpPressed()
{
	MovementState = EMovementState::Jump_State;
	MovementSpeedUpdate();	
	
	Jump();	
}
#endif

void AFugitiveCharacter::InputRunPressed()
{
	MovementState = EMovementState::Sprint_State;
	MovementSpeedUpdate();
}

void AFugitiveCharacter::InputRunReleased()
{
	MovementState = EMovementState::Walk_State;
	MovementSpeedUpdate();
}


#ifdef CROUCH_ENABLE
void AFugitiveCharacter::InputCrouchPressed()
{
	MovementState = EMovementState::Crouch_State;
	MovementSpeedUpdate();
	Crouch();
}

void AFugitiveCharacter::InputCrouchReleased()
{
	MovementState = EMovementState::Walk_State;
	MovementSpeedUpdate();
	UnCrouch();
}
#endif CROUCH_ENABLE

void AFugitiveCharacter::InputCameraAxis(float Value)
{
	if (Value == 0.f)
		return;

	float NewDistance;
	bool bIsResult = true;

	if (Value < 0)
	{
		NewDistance = CameraBoom->TargetArmLength + MoveCameraDistance;
		if (MaxCameraDistance < NewDistance)
			bIsResult = false;
	}
	else
	{
		NewDistance = CameraBoom->TargetArmLength - MoveCameraDistance;
		if (MinCameraDistance > NewDistance)
			bIsResult = false;
	}

	MoveToCameraDistance = NewDistance;
	bIsCameraMovesAway = bIsResult;
}

void AFugitiveCharacter::MovementSpeedUpdate()
{
	float ResSpeed = 600.f;

	switch (MovementState) {
	case EMovementState::Aim_State:
		ResSpeed = CharacterSpeed.AimSpeed;
		break;
	case EMovementState::Walk_State:
		ResSpeed = CharacterSpeed.WalkSpeed;
		break;
	case EMovementState::Backwalk_State:
		ResSpeed = CharacterSpeed.BackwalkSpeed;
		break;
	case EMovementState::Sprint_State:
		ResSpeed = CharacterSpeed.SprintSpeed;
		break;
	case EMovementState::Crouch_State:
		ResSpeed = CharacterSpeed.CrouchSpeed;
		GetCharacterMovement()->MaxWalkSpeedCrouched = ResSpeed;
		break;
	case EMovementState::Jump_State:
		ResSpeed = CharacterSpeed.JumpSpeed;		
		break;

	default:
		break;
	}

	GetCharacterMovement()->MaxWalkSpeed = ResSpeed;

	// Update weapon movement state for setting disperstions
	if (CurrentWeapon)
		CurrentWeapon->UpdateStateWeapon(MovementState);
}

void AFugitiveCharacter::MoveToInteractActor(AActor* TargetActor, float Distance)
{
	UE_LOG(LogTemp, Warning, TEXT("%s - XX:%f, %f"), TEXT(__FUNCTION__), MinDistanceToInteractActor, Distance);
	if (MinDistanceToInteractActor <= Distance)
	{
		// Мы слишком далеко, надо подойти
		bIsMovedToInteractionActor = true;
		MyTargetInteractActor = TargetActor;		
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("%s - 00:%f, %f"), TEXT(__FUNCTION__), MinDistanceToInteractActor, Distance);
		if (MyPlayerController)
		{
			UE_LOG(LogTemp, Warning, TEXT("%s - 01:%f, %f"), TEXT(__FUNCTION__), MinDistanceToInteractActor, Distance);
			MyPlayerController->OnInteractActionStart();
		}
	}
}

#ifdef CAMERAMOVEAWAY_ENABLE
void AFugitiveCharacter::CameraMoveAway(float DeltaSeconds)
{
	// Distance to reach
	const float Dist =  MoveToCameraDistance - CameraBoom->TargetArmLength;

	int32 CameraDistanceInt = CameraBoom->TargetArmLength;
	if ((int32)CameraDistanceInt == (int32)MoveToCameraDistance)
	{
		bIsCameraMovesAway = false;
	}

	const float DeltaMove = Dist * FMath::Clamp<float>(DeltaSeconds * 10.f, 0.f, 1.f);
		
	float CameraDistance = CameraBoom->TargetArmLength + DeltaMove;
	CameraBoom->TargetArmLength = CameraDistance;
}
#endif CAMERAMOVEAWAY_ENABLE

UT_InventoryComponent* AFugitiveCharacter::GetInventory()
{
	return InventoryComponent;
}

void AFugitiveCharacter::OnFootStep()
{
	float HearingRange = 0;
	
	switch(MovementState)
	{
#ifdef CROUCH_ENABLE	
		case EMovementState::Crouch_State:
			return;
#endif CROUCH_ENABLE

		case EMovementState::Aim_State:
		case EMovementState::Backwalk_State:
		case EMovementState::Walk_State:
			HearingRange = WalkHearingRange; 
			break;

#ifdef JUMP_ENABLE
		case EMovementState::Jump_State:
#endif JUMP_ENABLE
		
		case EMovementState::Sprint_State:
			HearingRange = RunHearingRange; 
			break;
	}
	
	UAISense_Hearing::ReportNoiseEvent(GetWorld(), GetActorLocation(), 1.f, this,
										HearingRange, NAME_None);
}

void AFugitiveCharacter::HealthRecover(float Value)
{
	if (HealthComponent)
	{
		HealthComponent->Recover(Value);
	}
}

// Выбор оружия размещенного в одной из рук
void AFugitiveCharacter::InputSelectHandWeapon(uint8 InventorySlot)
{
	if (!InventoryComponent)
		return;

	FName WeaponName = NAME_None;
	uint8 RoundInMagazine = 0;
	if (InventoryComponent->GetWeaponFromHand(InventorySlot, WeaponName, RoundInMagazine))
	{
		if (CurrentWeapon && CurrentWeapon->IsReload())
		{
			CurrentWeapon->CancelReload();
		}

		CharacterInitWeapon(WeaponName, RoundInMagazine);
		SelectedWeaponHeadSlot = InventorySlot;
	}
}

void AFugitiveCharacter::CheckMoveToActorFinish()
{
	float Distance = MyPlayerController->GetNavigationPathLength(GetActorLocation(),
													MyTargetInteractActor->GetActorLocation());
	if (MinDistanceToInteractActor >= Distance)
	{
		bIsMovedToInteractionActor = false;
		MyTargetInteractActor = nullptr;
		MyPlayerController->OnInteractActionStart();
	}
}

void AFugitiveCharacter::InputWeaponReload()
{
	if (!CurrentWeapon)
		return;

	// -1 потому что <= в tryreload
	CurrentWeapon->TryReload(CurrentWeapon->WeaponInfo.MaxRoundInMagazine - 1);
	
}

void AFugitiveCharacter::OnChangeWeaponMagazine(uint8 Rounds)
{
	if (!InventoryComponent)
		return;

	InventoryComponent->UpdateWeaponItem(SelectedWeaponHeadSlot, Rounds);
}

FVector2D AFugitiveCharacter::GetPawnMoveDirection()
{
	FVector2D MoveDirection = FVector2D::ZeroVector;
	if (MyPlayerController)
		MoveDirection = MyPlayerController->GetInputMoveDirection();
	return MoveDirection;
}

// Удаляем оружие из руки. Используется при смене оружия в руке или переносе в инвентаре
void AFugitiveCharacter::BreakWeaponHead(uint8 InventorySlot)
{
	if (InventorySlot != SelectedWeaponHeadSlot && CurrentWeapon == nullptr)
		return;

	CurrentWeapon->Destroy();
	CurrentWeapon = nullptr;	
}

void AFugitiveCharacter::CharacterInitWeapon(FName IdWeaponName, uint8 RoundInMagazine)
{
	UMyGameInstance* myGI = Cast<UMyGameInstance>(GetGameInstance());

	if (!myGI)
		return;

	FWeaponInfo myWeaponInfo;

	if (!myGI->GetWeaponInfoByName(IdWeaponName, myWeaponInfo))
		return;
	
	if (CurrentWeapon && myWeaponInfo.WeaponType != EWeaponTypes::Grenade)
	{
		CurrentWeapon->Destroy();
		CurrentWeapon = nullptr;
	}
	
	if (myWeaponInfo.WeaponClass)
	{
		FVector SpawnLocation = FVector(0);
		FRotator SpawnRotation = FRotator(0);

		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		SpawnParams.Owner = this;
		SpawnParams.Instigator = GetInstigator();

		AWeaponDefault* myWeapon = Cast<AWeaponDefault>(GetWorld()->SpawnActor(myWeaponInfo.WeaponClass, &SpawnLocation, &SpawnRotation, SpawnParams));
		if (myWeapon)
		{
			myWeapon->InitWeapon(GetMesh(), myWeaponInfo, MovementState, RoundInMagazine);
			myWeapon->OnChangeWeaponMagazine.AddDynamic(this, &AFugitiveCharacter::OnChangeWeaponMagazine);

			CurrentWeapon = myWeapon;
		}
	}
	else
		UE_LOG(LogTemp, Warning, TEXT("%s - Weapon not found in table (NULL)"), TEXT(__FUNCTION__));		
}

uint8 AFugitiveCharacter::GetWeaponRound()
{	
	if (CurrentWeapon)
	{
		return CurrentWeapon->GetWeaponRound();
	}

	return 0;		
}

void AFugitiveCharacter::OnGatheringCPP_Implementation(FItemInfo ItemInfo, AActor* TargetActor)
{
	// IN BP TEST
}

void AFugitiveCharacter::OnHandAttack_Implementation()
{
	// IN BP
}


// Когда жизни закончились
void AFugitiveCharacter::OnEmptyHealth()
{
	bIsAlive = false;
	if (MyPlayerController)
	{
		MyPlayerController->SetCanMove(false);
		OnDead();
	}
	else
		UE_LOG(LogTemp, Error, TEXT("%s - Player is die, but Player controller not found!"), TEXT(__FUNCTION__));		
}

void AFugitiveCharacter::OnDead()
{
	float TimeAnimation = 1.f;
	
	int32 rnd = FMath::RandHelper(DeadsAnim.Num()); 
	if (DeadsAnim.IsValidIndex(rnd) && DeadsAnim[rnd] && GetMesh() && GetMesh()->GetAnimInstance())
	{
		TimeAnimation = DeadsAnim[rnd]->GetPlayLength();
		GetMesh()->GetAnimInstance()->Montage_Play(DeadsAnim[rnd]);
	}

	UnPossessed();
	this->Tags.Remove(FName("Player"));

	// Timer ragdoll
	GetWorldTimerManager().SetTimer(RagdollTimerHandle, this, &AFugitiveCharacter::EnableRagdoll,
                                        TimeAnimation, false);

}

void AFugitiveCharacter::EnableRagdoll()
{
	if (GetMesh())
	{
		GetCharacterMovement()->Deactivate();
		//RootComponent->ComponentTags.Remove(FName("Player"));
		
		GetMesh()->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
		GetMesh()->SetSimulatePhysics(true);
	}
}

void AFugitiveCharacter::CheckMoveGatheringCancel()
{
	if (GatheringActor == nullptr)
		return;

	FVector ActorLocation = GetActorLocation();
	FVector TargetLocation = GatheringActor->GetActorLocation();
	if ((ActorLocation - TargetLocation).Size() < MinDistanceToInteractActor && MyPlayerController)
	{
		UAIBlueprintHelperLibrary::SimpleMoveToLocation(MyPlayerController, ActorLocation);
		OnGathering(GatheringInfo, GatheringActor);
	}
}

void AFugitiveCharacter::OnGathering_Implementation(FItemInfo ItemInfo, AActor* TargetActor)
{
//todo:	OnGathering1 вызывается из нескольких мест
	bIsGathering = true;
	GatheringInfo = ItemInfo;

	FVector ActorLocation = GetActorLocation();
	FVector TargetLocation = TargetActor->GetActorLocation();

	float Distance = (ActorLocation - TargetLocation).Size();
	if (MinDistanceToInteractActor > Distance)
	{
		if (MyPlayerController)
		{
			MyPlayerController->SetNewMoveDestination(TargetLocation);
		}
	}
	else
	{
		GatheringActor = nullptr;
		OnGatheringCPP(ItemInfo, TargetActor);
	}
	
}

void AFugitiveCharacter::OnStopGathering()
{
	if (bIsGathering)
	{
		GetMesh()->GetAnimInstance()->StopAllMontages(false);
		GatheringActor = nullptr;
		bIsGathering = false;
	}
}
