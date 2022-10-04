// Fill out your copyright notice in the Description page of Project Settings.


#include "HandWeaponWidget.h"

bool UHandWeaponWidget::Initialize()
{
	bool bInit = Super::Initialize();
	return bInit;
}

void UHandWeaponWidget::MakeWeaponSlotsStructure()
{
	FWeaponSlots TempWeaponStruct;

	TempWeaponStruct.WeaponImage = ItemImage_1;
	TempWeaponStruct.BackgroundImage = ItemBackGround_1;
	TempWeaponStruct.SizeBox = SizeBoxItem_1;
	TempWeaponStruct.WeaponCout = WeaponCout_1;
	WeaponSlots.Add(TempWeaponStruct);

	TempWeaponStruct.WeaponImage = ItemImage_2;
	TempWeaponStruct.BackgroundImage = ItemBackGround_2;
	TempWeaponStruct.SizeBox = SizeBoxItem_2;
	TempWeaponStruct.WeaponCout = WeaponCout_2;
	WeaponSlots.Add(TempWeaponStruct);

}
