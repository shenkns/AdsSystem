// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Data/ShopItemData.h"

#include "AdsShopItemData.generated.h"

UCLASS()
class ADSSYSTEM_API UAdsShopItemData : public UShopItemData
{
	GENERATED_BODY()

public:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Shop|Ads")
	bool bFreeWithDisabledAds;
};
