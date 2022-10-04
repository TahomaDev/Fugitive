// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Blueprint/UserWidget.h"
#include "Components/Image.h"
#include "Components/SizeBox.h"
#include "Components/TextBlock.h"

#include "HandWeaponWidget.generated.h"

USTRUCT(BlueprintType)
struct FWeaponSlots 
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UImage* WeaponImage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UImage* BackgroundImage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USizeBox* SizeBox;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTextBlock* WeaponCout;
};

UCLASS(BlueprintType, Blueprintable)
class FUGITIVE_API UHandWeaponWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual bool Initialize() override;

public:
	//Binds
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	class UImage*	ItemBackGround_1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	class UImage*	ItemBackGround_2;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	class UImage*	ItemImage_1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	class UImage*	ItemImage_2;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	class USizeBox* SizeBoxItem_1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	class USizeBox* SizeBoxItem_2;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	class UTextBlock* WeaponCout_1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	class UTextBlock* WeaponCout_2;

public:

	UFUNCTION(BlueprintCallable)
	void MakeWeaponSlotsStructure();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="CPP")
	TArray<FWeaponSlots>	WeaponSlots;
};
