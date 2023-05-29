// Copyright shenkns Ads System Developed With Unreal Engine. All Rights Reserved 2023.

#include "Managers/AdsManager.h"

#include "AppLovinProxy.h"
#include "ManagersSystem.h"
#include "../../../../../MobileStorePurchaseSystem/Source/MobileStorePurchaseSystem/Public/Data/MobileStorePurchaseShopItemData.h"
#include "../../../../../SaveLoadSystem/Source/SaveLoadSystem/Public/Managers/StatsManager.h"
#include "Data/ShopItemData.h"
#include "Kismet/GameplayStatics.h"
#include "Module/AdsSystemSettings.h"
#include "Stats/StatShopHistory.h"

void UAdsManager::InitManager()
{
	Super::InitManager();

	if(!UAppLovinProxy::GetApplovin()) return;
	UAppLovinProxy::GetApplovin()->Init();

	UAppLovinProxy::GetApplovin()->OnRewardedVideoEvent.AddUniqueDynamic(this, &UAdsManager::OnRewarded);
	UAppLovinProxy::GetApplovin()->OnRewardedVideoErrorEvent.AddUniqueDynamic(this, &UAdsManager::OnRewardedError);
	UAppLovinProxy::GetApplovin()->OnInterstitialEvent.AddUniqueDynamic(this, &UAdsManager::OnInterstitial);
	UAppLovinProxy::GetApplovin()->OnInterstitialErrorEvent.AddUniqueDynamic(this, &UAdsManager::OnInterstitialError);

	StartLoadLoadAds();
}

FString UAdsManager::GetRewardedPlacement() const
{
	if(const UAdsSystemSettings* Settings = GetDefault<UAdsSystemSettings>())
	{
		if(Settings->RewardedPlacements.Contains(UGameplayStatics::GetPlatformName()))
		{
			return *Settings->RewardedPlacements.Find(UGameplayStatics::GetPlatformName());
		}
	}

	return FString();
}

FString UAdsManager::GetInterstitialPlacement() const
{
	if(const UAdsSystemSettings* Settings = GetDefault<UAdsSystemSettings>())
	{
		if(Settings->InterstitialPlacements.Contains(UGameplayStatics::GetPlatformName()))
		{
			return *Settings->InterstitialPlacements.Find(UGameplayStatics::GetPlatformName());
		}
	}

	return FString();
}

bool UAdsManager::LoadInterstitial()
{
	if(!IsAdsEnabled()) return false;
	
	if(UAppLovinProxy* Proxy = UAppLovinProxy::GetApplovin())
	{
		Proxy->LoadInterstitial(GetInterstitialPlacement());

		return true;
	}

	return false;
}

bool UAdsManager::LoadRewarded()
{
	if(UAppLovinProxy* Proxy = UAppLovinProxy::GetApplovin())
	{
		Proxy->LoadRewardedVideo(GetRewardedPlacement());

		return true;
	}

	return false;
}

bool UAdsManager::ShowRewarded()
{
	if(UAppLovinProxy* Proxy = UAppLovinProxy::GetApplovin())
	{
		Proxy->ShowRewardedVideo(GetRewardedPlacement());

		return true;
	}

	return false;
}

bool UAdsManager::ShowInterstitial()
{
	if(!IsAdsEnabled()) return false;
	
	if(UAppLovinProxy* Proxy = UAppLovinProxy::GetApplovin())
	{
		Proxy->ShowInterstitial(GetInterstitialPlacement());

		return true;
	}

	return false;
}

bool UAdsManager::IsAdsEnabled() const
{
	const UAdsSystemSettings* AdsSystemSettings = GetDefault<UAdsSystemSettings>();
	if(!AdsSystemSettings) return false;
	
	const UManagersSystem* ManagersSystem = GetManagerSystem();
	if(!ManagersSystem) return true;

	const UStatsManager* StatsManager = ManagersSystem->GetManager<UStatsManager>();
	if(!StatsManager) return true;

	const UStatShopHistory* StatShopHistory = StatsManager->GetStat<UStatShopHistory>();
	if(!StatShopHistory) return true;

	const TArray<FPurchaseData> Purchases = StatShopHistory->GetAllPurchases(UShopItemData::StaticClass());

	for(const FPurchaseData Purchase : Purchases)
	{
		if(Purchase.ShopItem)
		{
			if(AdsSystemSettings->bDisableAdsOnStorePurchase && Cast<UMobileStorePurchaseShopItemData>(Purchase.ShopItem))
			{
				return false;
			}

			for(const TSoftObjectPtr<UShopItemData>& SoftItem : AdsSystemSettings->NoAdsShopItems)
			{
				if(SoftItem.LoadSynchronous()->Tag == Purchase.ShopItem->Tag)
				{
					return false;
				}
			}
		}
	}

	return true;
}

void UAdsManager::StartLoadLoadAds()
{
	if(IsAdsEnabled())
	{
		LoadInterstitial();
	}
	
	LoadRewarded();
}

void UAdsManager::OnRewarded(EAppLovinRewardedVideoEventType EventType)
{
	switch(EventType)
	{
	case EAppLovinRewardedVideoEventType::Closed:
		{
			LoadRewarded();
			break;
		}
	default:
		{
			break;
		}
	}
}

void UAdsManager::OnInterstitial(EAppLovinInterstitialEventType EventType)
{
	switch (EventType)
	{
	case EAppLovinInterstitialEventType::Closed:
		{
			LoadInterstitial();
			break;
		}
	default:
		{
			break;
		}
	}
}

void UAdsManager::OnRewardedError(EAppLovinRewardedErrorEventType EventType, int Code, FString Message)
{
	LoadRewarded();
}

void UAdsManager::OnInterstitialError(EAppLovinInterstitialErrorEventType EventType, int Code, FString Message)
{
	LoadInterstitial();
}
