// Fill out your copyright notice in the Description page of Project Settings.


#include "FugitiveHUD.h"
#include "Engine/Canvas.h"

void AFugitiveHUD::BeginPlay()
{
	Super::BeginPlay();
	Player = GetOwningPawn();

	Enemies.Empty();
}

void AFugitiveHUD::DrawHUD()
{
	Super::DrawHUD();

	if (!bIsMinimapShow || !MinimapBackground)
		return;

	if (IsCanvasValid_WarnIfNot() && GetOwningPawn() && GetOwningPawn()->IsLocallyControlled())
	{		
		CanvasSizeX = Canvas->SizeX * Canvas->GetDPIScale();
		CanvasSizeY = Canvas->SizeY * Canvas->GetDPIScale();

		MinimapCenterX = Canvas->SizeX * MinimapStartLocation.X * Canvas->GetDPIScale();
		MinimapCenterY = Canvas->SizeY * MinimapStartLocation.Y * Canvas->GetDPIScale();

		DrawBackground();
		DrawPlayerPosition();
		DrawObjectPosition(SimpleEnemies, SimpleEnemyTexture, SimpleEnemyScale);
		DrawObjectPosition(NPCs, NPCTexture, NPCsyScale);
		DrawObjectPosition(Pickups, PickupTexture, PickupScale);
		
	}
	else
		UE_LOG(LogTemp, Error, TEXT("%s - can't find canvas"), TEXT(__FUNCTION__));
}

void AFugitiveHUD::AddObjectOnMinimap(AActor* Object, EMinimapObjectType MinimapObjectType)
{
	if (!Object)
		return;
	
	switch(MinimapObjectType) {
		case EMinimapObjectType::SimpleEnemy:
			SimpleEnemies.Add(Object);
			break;
		
		case EMinimapObjectType::NPC:
			NPCs.Add(Object);
			break;

		case EMinimapObjectType::Pickup:
			Pickups.Add(Object);
			break;
	}
}

void AFugitiveHUD::RemoveObjectOnMinimap(AActor* Object, EMinimapObjectType MinimapObjectType)
{
	if (!Object)
		return;
	
	switch (MinimapObjectType) {
		case EMinimapObjectType::SimpleEnemy:
			SimpleEnemies.Remove(Object);
			break;
		
		case EMinimapObjectType::NPC:
			NPCs.Remove(Object);
			break;

		case EMinimapObjectType::Pickup:
			Pickups.Remove(Object);
			break;
	
	}
}

void AFugitiveHUD::DrawBackground()
{
	float X = MinimapCenterX - MinimapSize * Canvas->GetDPIScale();
	float Y = MinimapCenterY - MinimapSize * Canvas->GetDPIScale();

	DrawTextureSimple(MinimapBackground, X, Y, BackgroundScale);
}

void AFugitiveHUD::DrawPlayerPosition()
{
	if (PlayerTexture)
	{
		DrawObject(PlayerTexture, MinimapCenterX, MinimapCenterY, PlayerScale,GetOwningPawn()->GetActorRotation().Yaw);
	}
}

void AFugitiveHUD::DrawObjectPosition(TArray<AActor*> OtherActors, UTexture* Texture, float Scale)
{
	if (!Player || OtherActors.Num() == 0 || !Texture)
		return;

	FVector PlayerLocation = Player->GetActorLocation();
	
	for (int i=0; i<OtherActors.Num(); i++)
	{
		float X,Y;
		
		if (OtherActors[i])
		{
			// Check range of enemy
			FVector ObjectLocation = OtherActors[i]->GetActorLocation();			
			float HorizontalDistance = (PlayerLocation - ObjectLocation).Size2D();
		
			if (HorizontalDistance <= MinimapRange)
			{
				GetRadarDotPosition(PlayerLocation, OtherActors[i], X, Y);
				float Size = MinimapSize * Canvas->GetDPIScale();
				float ScreenY = FMath::Clamp(X, Size*-1, Size ) + MinimapCenterY;
				float ScreenX = FMath::Clamp(Y, Size*-1, Size) + MinimapCenterX;
				float OtherActorYawRotation = OtherActors[i]->GetActorRotation().Yaw;

				DrawObject(Texture, ScreenX, ScreenY, Scale,OtherActorYawRotation);
			}
		}
	}
}

void AFugitiveHUD::DrawObject(UTexture* Texture, float ScreenX, float ScreenY, float Scale, float Rotate)
{
	FCanvasTileItem TileItem(FVector2D(ScreenX, ScreenY), Texture->Resource, FLinearColor::White);

	TileItem.Rotation = FRotator(0, Rotate, 0);
	TileItem.PivotPoint = FVector2D(0.5f);
	
	TileItem.Size *= Scale;
	TileItem.BlendMode = SE_BLEND_Translucent;
	Canvas->DrawItem(TileItem);
}

void AFugitiveHUD::GetRadarDotPosition(FVector PlayerLocation, AActor* OtherActor, float& X, float& Y)
{
	FVector TargetLocation = OtherActor->GetActorLocation();
	FTransform TmpTransform = FTransform(FQuat(0,0,0,0),
										PlayerLocation,
										FVector(1.f));
	
	FVector InverseVector = TmpTransform.InverseTransformPosition(TargetLocation) /  ObjectDistanceScale;

	X = InverseVector.X * -1.f;
	Y = InverseVector.Y;
}
