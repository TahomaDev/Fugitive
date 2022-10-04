// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Fugitive/Types.h"
#include "GameFramework/HUD.h"
#include "FugitiveHUD.generated.h"

UCLASS(config=Game, hidecategories=(Rendering,Actor,Input,Replication), showcategories=("Input|MouseInput", "Input|TouchInput"), notplaceable, transient, BlueprintType, Blueprintable)
class FUGITIVE_API AFugitiveHUD : public AHUD
{
	GENERATED_BODY()

protected:
	
	void BeginPlay() override;
	void DrawHUD() override;
	
	float CanvasSizeX = 0;
	float CanvasSizeY = 0;

	APawn* Player = nullptr;

private:
	// Functions
	void DrawBackground();
	void DrawPlayerPosition();
	void DrawObject(UTexture* Texture, float ScreenX, float ScreenY, float Scale, float Rotate);
	void DrawObjectPosition(TArray<AActor*> OtherActors, UTexture* Texture, float Scale);
	void GetRadarDotPosition(FVector PlayerLocation, AActor* OtherActor, float &X, float &Y);

	// Objects
	TArray<AActor*> SimpleEnemies;
	TArray<AActor*> NPCs;
	TArray<AActor*> Pickups;

	// Values
	TArray<AActor> Enemies;

	float MinimapCenterX = 0.f;
	float MinimapCenterY = 0.f;

public:

	UFUNCTION(BlueprintCallable)
    void AddObjectOnMinimap(AActor* Object, EMinimapObjectType MinimapObjectType);
	UFUNCTION(BlueprintCallable)
	void RemoveObjectOnMinimap(AActor* Object, EMinimapObjectType MinimapObjectType);
	


	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Minimap ")
	FVector2D MinimapStartLocation = FVector2D(0.90f, 0.75f);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Minimap ")
	float MinimapSize = 100;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Minimap ")
	float BackgroundScale = 0.54;

	
	// Показывать миникарту или нет
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Minimap ")
	bool bIsMinimapShow = true;

	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Minimap ")
	float MinimapRange = 50000.0f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Minimap ")
	FVector2D Screen2D = FVector2D(10.0f, 10.f);;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Minimap ")
	float ObjectDistanceScale = 50.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Minimap ")
	float PlayerScale = 0.05f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Minimap ")
	float SimpleEnemyScale = 0.07f;	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Minimap ")
	float NPCsyScale = 0.05f;	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Minimap ")
	float PickupScale = 0.03f;	
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Minimap | Textures")
	UTexture* MinimapBackground = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Minimap | Textures")
	UTexture* SimpleEnemyTexture = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Minimap | Textures")
	UTexture* NPCTexture = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Minimap | Textures")
	UTexture* PlayerTexture = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Minimap | Textures")
	UTexture* PickupTexture = nullptr;
};
