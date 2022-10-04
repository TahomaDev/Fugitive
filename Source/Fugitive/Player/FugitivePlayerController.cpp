// Copyright Epic Games, Inc. All Rights Reserved.

#include "FugitivePlayerController.h"

#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "Runtime/Engine/Classes/Components/DecalComponent.h"
#include "FugitiveCharacter.h"
#include "AIController.h"
#include "DrawDebugHelpers.h"
#include "NavigationPath.h"
#include "NavigationSystem.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Fugitive/Core/Interfaces/IObjectReact.h"
#include "Fugitive/Core/Components/T_InventoryComponent.h"
#include "Kismet/KismetSystemLibrary.h"


AFugitivePlayerController::AFugitivePlayerController()
{
	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Crosshairs;
}

void AFugitivePlayerController::BeginPlay()
{
	GetMyPawn();

	if (MainGamePlayWidget)
		OnCreateMainGameWidget();
}

/*
void AFugitivePlayerController::CursorHittedToItem(AActor* HitActor)
{
	//!!! The parent function must check the validity of the HitActor !!!	
	if (!GetMyPawn())
		return;
		
	if (HitActor->GetClass()->ImplementsInterface(UIActiveItemReact::StaticClass()))
	{
		ItemAction = IIActiveItemReact::Execute_GetItemReact(HitActor, MyPawn->GetParentActor());
		ActualCursorHitActor = HitActor;
		
		switch(ItemAction)
		{
		case EItemAction::Pickup_ItemAction:
			CurrentMouseCursor = EMouseCursor::GrabHand;
			break;
		case EItemAction::Used_ItemAction:
			CurrentMouseCursor = EMouseCursor::CardinalCross;
			break;
		default:
			ItemAction = EItemAction::None_ItemAction;
			ActualCursorHitActor = nullptr;
			CurrentMouseCursor = EMouseCursor::Default;
		}

		if (bMoveToMouseCursor)
			ActualCursorHitActor = HitActor;
		
		UE_LOG(LogTemp, Warning, TEXT("AFugitivePlayerController::CursorHittedToItem - GetItemAction: %d"), ItemAction);
	}
}
*/
/*
bool AFugitivePlayerController::CanApplyItem()
{
	bool bIsResult = false;
	if (MoveToActor && GetMyPawn())
	{
		FVector PawnLocation = MyPawn->GetActorLocation();
		FVector TargetLocation = MoveToActor->GetActorLocation();

		if (PickupDistance >= FVector::Dist(PawnLocation, TargetLocation))
			bIsResult = true;
					
		UE_LOG(LogTemp, Warning, TEXT("AFugitivePlayerController::MoveToMouseCursor - location %f"), FVector::Dist(PawnLocation, TargetLocation));
			
	}
	return bIsResult;
}*/

APawn* AFugitivePlayerController::GetMyPawn()
{
	if (IsValid(MyPawn))
		return MyPawn;

	MyPawn = GetPawn();
	return MyPawn;
}

/*
void AFugitivePlayerController::ApplyItem()
{
	MyPawn = GetMyPawn();
	AFugitiveCharacter* MyChar = Cast<AFugitiveCharacter>(MyPawn);
	if (!MyChar)
		return;

	UT_InventoryComponent* InventoryComponent = MyChar->GetInventory();
	if (!InventoryComponent)
		return;
	
	FName ItemName;
	EInventoryItemType ItemType = EInventoryItemType::LootItem;
	float Result = IIActiveItemReact::Execute_GetItem(MoveToActor, MyPawn->GetParentActor(), ItemName, ItemType);
	
	if (ItemAction == EItemAction::Pickup_ItemAction)
		PickupItem(InventoryComponent, ItemName, ItemType);
	else if (ItemAction == EItemAction::Used_ItemAction)
		UseItem(ItemName);
	
	MoveToActor = nullptr;
}*/
/*
void AFugitivePlayerController::PickupItem(UT_InventoryComponent* InventoryComponent, FName ItemName, EInventoryItemType ItemType)
{
	bool IsOneItemBySlot = (ItemType == EInventoryItemType::WeaponItem) ? true : false;
	
	if (InventoryComponent->AddItem(ItemName, ItemType, IsOneItemBySlot))
		IIActiveItemReact::Execute_Pickup(MoveToActor, MyPawn->GetParentActor());
}*/

void AFugitivePlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);

	if (CanMove)
	{
		FHitResult TraceHitResult;
		TArray< TEnumAsByte<EObjectTypeQuery> > ObjectTypes;
		
		ObjectTypes.Add(EObjectTypeQuery::ObjectTypeQuery2); // WorldDynamic
		
		if (GetHitResultUnderCursorForObjects(ObjectTypes, true, TraceHitResult))
		{
			AActor* HitActor = TraceHitResult.GetActor();
			if (HitActor != nullptr
				&& UKismetSystemLibrary::DoesImplementInterface( HitActor, UIObjectReact::StaticClass() )
				&& IIObjectReact::Execute_HaveYouCustomDephForSelect(HitActor))
			{
				if (HitActor != SelectableActor)
				{
					if (SelectableActor != nullptr)
						IIObjectReact::Execute_CustomDephForSelectEnable(SelectableActor, false);

					SelectableActor = HitActor;
					IIObjectReact::Execute_CustomDephForSelectEnable(HitActor, true);
				}
			}
			else
			{
				if (SelectableActor != nullptr)
				{
					IIObjectReact::Execute_CustomDephForSelectEnable(SelectableActor, false);
					SelectableActor = nullptr;
				}
			}
		


			
			/*
			AActor* HitActor = TraceHitResult.GetActor();
			if (HitActor != nullptr )
			{
				if (HitActor != MoveToActor && HitActor != OldCursorHitActor)
				{
					OldCursorHitActor = HitActor;
					CursorHittedToItem(HitActor);
				}				
			}
			else
			{
				ActualCursorHitActor = nullptr;
				OldCursorHitActor = nullptr;
				CurrentMouseCursor = EMouseCursor::Default;
			}
		
			// keep updating the destination every tick while desired
			if (TraceHitResult.bBlockingHit && bMoveToMouseCursor)
			{
				
				if (CanApplyItem())
				{					
					ApplyItem();
				}
				else				
					SetNewMoveDestination(TraceHitResult.ImpactPoint);
			}
			*/
		}
		else if (SelectableActor != nullptr)
		{
			IIObjectReact::Execute_CustomDephForSelectEnable(SelectableActor, false);
			SelectableActor = nullptr;
		}
		
	}

	/*
	if (CanApplyItem())
	{
		ApplyItem();
	}
	*/
}

void AFugitivePlayerController::SetupInputComponent()
{
	// set up gameplay key bindings
	Super::SetupInputComponent();

	UWidgetBlueprintLibrary::SetInputMode_GameAndUIEx(this, nullptr, EMouseLockMode::DoNotLock, true);
	SetShowMouseCursor(true);
	
	// move
	InputComponent->BindAxis(TEXT("MoveForward"), this, &AFugitivePlayerController::MoveToAxisX);
	InputComponent->BindAxis(TEXT("MoveRight"), this, &AFugitivePlayerController::MoveToAxisY);

//	InputComponent->BindAction("SetDestination", IE_Pressed, this, &AFugitivePlayerController::OnSetDestinationPressed);
//	InputComponent->BindAction("SetDestination", IE_Released, this, &AFugitivePlayerController::OnSetDestinationReleased);

	// support touch devices 
	InputComponent->BindTouch(EInputEvent::IE_Pressed, this, &AFugitivePlayerController::MoveToTouchLocation);
	InputComponent->BindTouch(EInputEvent::IE_Repeat, this, &AFugitivePlayerController::MoveToTouchLocation);
}

void AFugitivePlayerController::MoveToTouchLocation(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	if (!CanMove)
		return;
	
	FVector2D ScreenSpaceLocation(Location);

	// Trace to see what is under the touch location
	FHitResult HitResult;
	GetHitResultAtScreenPosition(ScreenSpaceLocation, CurrentClickTraceChannel, true, HitResult);
	if (HitResult.bBlockingHit)
	{
		// We hit something, move there
		SetNewMoveDestination(HitResult.ImpactPoint);
	}
}

void AFugitivePlayerController::SetNewMoveDestination(const FVector DestLocation)
{
	if (GetMyPawn())
	{
		float const Distance = FVector::Dist(DestLocation, MyPawn->GetActorLocation());

		// We need to issue move command only if far enough in order for walk animation to play correctly
		if ((Distance > 120.0f))
		{
			
			UAIBlueprintHelperLibrary::SimpleMoveToLocation(this, DestLocation);
		}
	}
}

float AFugitivePlayerController::GetNavigationPathLength(FVector ActorLocation, FVector TargetLocation)
{
	float fResult = 0.f;
	
	UNavigationPath* tpath;
	UNavigationSystemV1* NavSys = UNavigationSystemV1::GetCurrent(GetWorld());
     
	tpath = NavSys->FindPathToLocationSynchronously(GetWorld(), ActorLocation, TargetLocation);
         
	if (tpath != NULL)
	{
		fResult = tpath->GetPathLength();
/*		for (int pointiter = 0; pointiter < tpath->PathPoints.Num(); pointiter++)
		{
			DrawDebugSphere(GetWorld(), tpath->PathPoints[pointiter], 10.0f, 12, FColor(255, 0, 0));
		}*/
	}

	return fResult;
}

void AFugitivePlayerController::OnSetDestinationPressed()
{
	// set flag to keep updating destination until released
	bMoveToMouseCursor = CanMove;
}

void AFugitivePlayerController::OnSetDestinationReleased()
{
	// clear flag to indicate we should stop updating the destination
	bMoveToMouseCursor = false;
}

void AFugitivePlayerController::MoveToAxisX(float Value)
{
	AxisX = Value;
	if (AxisY != 0 || AxisX != 0)
		MoveToAxis();
}

void AFugitivePlayerController::MoveToAxisY(float Value)
{
	AxisY = Value;
	if (AxisY != 0 || AxisX != 0)
		MoveToAxis();
}

namespace
{
	UPathFollowingComponent* InitNavigationControl(AController& Controller)
	{
		AAIController* AsAIController = Cast<AAIController>(&Controller);
		UPathFollowingComponent* PathFollowingComp = nullptr;

		if (AsAIController)
		{
			PathFollowingComp = AsAIController->GetPathFollowingComponent();
		}
		else
		{
			PathFollowingComp = Controller.FindComponentByClass<UPathFollowingComponent>();
			if (PathFollowingComp == nullptr)
			{
				PathFollowingComp = NewObject<UPathFollowingComponent>(&Controller);
				PathFollowingComp->RegisterComponentWithWorld(Controller.GetWorld());
				PathFollowingComp->Initialize();
			}
		}

		return PathFollowingComp;
	}
}

void AFugitivePlayerController::MoveToAxis()
{	
	if (!CanMove || !GetMyPawn())
		return;

	/*	
	// Move with NavigationMesh bounds volume
	FVector Location = MyPawn->GetActorLocation();
	Location.X += AxisX*120;
	Location.Y += AxisY*120;

	UAIBlueprintHelperLibrary::SimpleMoveToLocation(this, Location);
	*/
	
	// Move without NavigationMesh bounds volume
	UPathFollowingComponent* PathFollowingComp = ::InitNavigationControl(*this);
	if (PathFollowingComp != nullptr && PathFollowingComp->GetStatus() != EPathFollowingStatus::Idle)
	{
		PathFollowingComp->RequestMoveWithImmediateFinish(EPathFollowingResult::Success);		
	}

/*	FVector ActorForwardVector = MyPawn->GetActorForwardVector();
	UE_LOG(LogTemp, Warning, TEXT("%s - [Forward] %f.%f.%f"), TEXT(__FUNCTION__),
		ActorForwardVector.X,
		ActorForwardVector.Y,
		ActorForwardVector.Z);
*/
	MyPawn->AddMovementInput(FVector(1.f, 0, 0), AxisX);
	MyPawn->AddMovementInput(FVector(0, 1.f, 0), AxisY);

	if (ItemLocation != FVector::ZeroVector)
	{
		FVector PawnLocation = MyPawn->GetActorLocation();
		float Dist = FVector::Distance(PawnLocation, ItemLocation);
		if (Dist > 50.f)
			OnCancelItemReact(Dist);		
	}
}

void AFugitivePlayerController::OnInteractActionStart_Implementation()
{
	// In BP
}

FVector2D AFugitivePlayerController::GetInputMoveDirection()
{
	return FVector2D(AxisX, AxisY);
}

bool AFugitivePlayerController::OnSwapInventoryItems(int ItemIndex1, int ItemIndex2)
{
	if (ItemIndex1 == ItemIndex2)
		return true;

	bool bIsResult = false;

	if (!GetMyPawn())
		return bIsResult;

	AFugitiveCharacter* MyPlayer = Cast<AFugitiveCharacter>(MyPawn);
	if (!MyPlayer)
		return bIsResult;

	UT_InventoryComponent* Inventory = MyPlayer->GetInventory();
	if (!Inventory)
		return bIsResult;

	return Inventory->SwapInventoryItems(ItemIndex1, ItemIndex2);
	
}

// Проверяем, есть ли приграды на пути сбора объекта
bool AFugitivePlayerController::CheckForObstacles(AActor* Target)
{
	bool bResult = true;
	
	if (!GetMyPawn() && !Target)
		return false;

	TArray<FHitResult> OutHits;
	FVector PawnLocation = MyPawn->GetActorLocation();
	PawnLocation.Z += 100.f;
	FVector TargetLocation = Target->GetActorLocation();
	TArray<AActor*> Ignored;
	Ignored.Add(this);
	Ignored.Add(GetOwner());

	if (UKismetSystemLibrary::LineTraceMulti(GetWorld(), PawnLocation, TargetLocation, ETraceTypeQuery::TraceTypeQuery1,
		false, Ignored,
		EDrawDebugTrace::None,
		OutHits, true, 
        FLinearColor::Blue, FLinearColor::Green, 1.f ))
	{
		for(int i=0; i < OutHits.Num(); i++)
		{
			if (OutHits[i].GetActor() && OutHits[i].GetActor() == Target)
			{
				bResult = true;
				break;
			}
			if (OutHits[i].bBlockingHit)
			{
				bResult = false;
				break;
			}
		}
	}

	return bResult;
	
}

void AFugitivePlayerController::OnCreateMainGameWidget_Implementation()
{
	UUserWidget* Widget = CreateWidget(this, MainGamePlayWidget);
	if (Widget)
	{
		MainGamePlayWidget_Ref = Widget;
		/*
		// При долгой загрузке заставляем HealthComponent ещё раз забрудкастить свою информацию
		
		if (GetPawn())
		{
			AFugitiveCharacter* MyPlayer = Cast<AFugitiveCharacter>(MyPawn);
			if (MyPlayer)
			{
				UT_HealthComponent* HealthComp = MyPlayer->GetHealthComponent();
				if (HealthComp)
					HealthComp->GetCurrentHealth();
			}
		}*/
		Widget->AddToViewport();
	}
}

void AFugitivePlayerController::OnCancelItemReact_Implementation(float Distance)
{
	// In BP
}
