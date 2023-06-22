// Copyright shenkns Ads System Developed With Unreal Engine. All Rights Reserved 2023.

#pragma once

#include "UObject/Object.h"

#include "AdsSystemSettings.generated.h"

class UShopItem;
class UShopItemData;

UCLASS(Config=Game, DefaultConfig)
class ADSSYSTEM_API UAdsSystemSettings : public UObject
{
	GENERATED_BODY()

public:

	UPROPERTY(EditDefaultsOnly, Config, Category = "Ads")
	TMap<FString, FString> InterstitialPlacements;

	UPROPERTY(EditDefaultsOnly, Config, Category = "Ads")
	TMap<FString, FString> RewardedPlacements;

	UPROPERTY(EditDefaultsOnly, Config, Category = "Ads")
	float AdsLoadRetryDelay = 5.f;
	
	UPROPERTY(EditDefaultsOnly, Config, Category = "Ads|NoAds")
	TArray<TSoftObjectPtr<UShopItemData>> NoAdsShopItems;

	UPROPERTY(EditDefaultsOnly, Config, Category = "Ads|NoAds")
	TArray<TSoftClassPtr<UShopItem>> NoAdsShopItemClasses;

	// Debug
	UPROPERTY(EditDefaultsOnly, Config, Category = "Debug")
	bool bShowDebugMessages;
};