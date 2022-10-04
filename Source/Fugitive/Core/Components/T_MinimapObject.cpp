// Fill out your copyright notice in the Description page of Project Settings.

#include "T_MinimapObject.h"

#include "Fugitive/Player/FugitivePlayerController.h"
#include "Fugitive/Player/HUD/FugitiveHUD.h"
#include "Kismet/GameplayStatics.h"

UT_MinimapObject::UT_MinimapObject()
{
	PrimaryComponentTick.bCanEverTick = false;
}


// Called when the game starts
void UT_MinimapObject::BeginPlay()
{
	Super::BeginPlay();
	AddToRadar();
}

void UT_MinimapObject::AddToRadar()
{
	if (!GetOwner() || !bDrawToMinimap)
		return;

	AFugitivePlayerController* OwnerPC = Cast<AFugitivePlayerController>( UGameplayStatics::GetPlayerController(GetWorld(), 0));
	if (OwnerPC)
	{
		AFugitiveHUD* OwnerHUD = Cast<AFugitiveHUD>(OwnerPC->GetHUD());
		if (OwnerHUD)
		{
			OwnerHUD->AddObjectOnMinimap(GetOwner(), MinimapObjectType);
		}
	}
}

void UT_MinimapObject::RemoveFromRadar()
{
	if (!GetOwner() || !bDrawToMinimap)
		return;
	
	AFugitivePlayerController* OwnerPC = Cast<AFugitivePlayerController>( UGameplayStatics::GetPlayerController(GetWorld(), 0));
	if (OwnerPC)
	{
		AFugitiveHUD* OwnerHUD = Cast<AFugitiveHUD>(OwnerPC->GetHUD());
		if (OwnerHUD)
		{
			OwnerHUD->RemoveObjectOnMinimap(GetOwner(), MinimapObjectType);
		}
	}
}
