// Fill out your copyright notice in the Description page of Project Settings.


#include "InteractionItem.h"


void AInteractionItem::BeginPlay()
{
	ItemInfo_Real = ItemInfo;
	Super::BeginPlay();
}

void AInteractionItem::OnKill()
{
	if (RecoveryTime > 0)
	{
		bIsInvisible = true;
		this->SetActorHiddenInGame(true);
		GetWorld()->GetTimerManager().SetTimer(ItemRecoveryTimerHandle, this,
            &AInteractionItem::ItemRecoveryTimer, RecoveryTime, false);
	}
	else
		Super::OnKill();
}

void AInteractionItem::ItemRecoveryTimer()
{
	bIsInvisible = false;
	this->SetActorHiddenInGame(false);
	ItemInfo = ItemInfo_Real;
	Super::AmountRandom();
}

EObjectAction AInteractionItem::HowAreYou_Implementation(AActor* Actor)
{
	return (bIsInvisible) ? EObjectAction::None : EObjectAction::Interaction;
}

bool AInteractionItem::GetInteractTime_Implementation(float &Time)
{
	Time = InteractTime;
	return true;
}
