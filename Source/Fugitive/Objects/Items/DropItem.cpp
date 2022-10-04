// Fill out your copyright notice in the Description page of Project Settings.


#include "DropItem.h"

void ADropItem::BeginPlay()
{
	bIsDropItem = true;
	Super::BeginPlay();
}

// Поиск первого блок предмета, чтобы разместить на нем выброшенный предмет (поиск земли)
void ADropItem::SetLocationToGround()
{
	FVector StartLocation = GetActorLocation();
	FVector EndLocation = StartLocation;
	EndLocation.Z -= 1000.f;

	FHitResult OutHit;
	TArray<AActor*> IgnoredActors;
	IgnoredActors.Add(this);
	IgnoredActors.Add(GetOwner());
	IgnoredActors.Add(GetInstigator());

	FCollisionQueryParams CollisionParams;
	CollisionParams.bFindInitialOverlaps = false;
	CollisionParams.bIgnoreBlocks = false;
	CollisionParams.AddIgnoredActors(IgnoredActors);
	
	bool ResultLineTrace = GetWorld()->LineTraceSingleByChannel(OutHit,
        StartLocation, EndLocation,
        ECollisionChannel::ECC_WorldStatic, CollisionParams);

	if (ResultLineTrace && OutHit.bBlockingHit)
	{
		EndLocation = OutHit.Location;
		EndLocation.Z += 15.f;
		SetActorLocation(EndLocation);
	}
	else
		UE_LOG(LogTemp, Error, TEXT("%s - Can't find ground"), TEXT(__FUNCTION__));
	
}

void ADropItem::Init(FItemInfo NewItemInfo)
{
	ItemInfo = NewItemInfo;

	VisualMeshInit();
	CheckValidMeshes();
	SetLocationToGround();
}
