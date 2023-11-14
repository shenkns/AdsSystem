// Copyright shenkns Ads System Developed With Unreal Engine. All Rights Reserved 2023.

#include "Managers/AdsManager.h"

#include "AppLovinProxy.h"
#include "Log.h"
#include "ManagersSystem.h"
#include "Managers/StatsManager.h"
#include "Data/ShopItemData.h"
#include "Kismet/GameplayStatics.h"
#include "Log/Details/LocalLogCategory.h"
#include "Managers/ShopManager.h"
#include "Module/AdsSystemModule.h"
#include "Module/AdsSystemSettings.h"
#include "Stats/StatShopHistory.h"

DEFINE_LOG_CATEGORY_LOCAL(LogAdsSystem);

void UAdsManager::InitManager()
{
	Super::InitManager();

	if(!UAppLovinProxy::GetApplovin()) return;
	UAppLovinProxy::GetApplovin()->Init();

	LOG(Display, """{}"" AppLovin Proxy Initialized", UAppLovinProxy::GetApplovin());

	BindShopEvents();

	BindAppLovinEvents();
	
	StartLoadAds();
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
	if(!IsAdsEnabled() || IsInterstitialLoaded()) return false;
	
	if(UAppLovinProxy* Proxy = UAppLovinProxy::GetApplovin())
	{
		Proxy->LoadInterstitial(GetInterstitialPlacement());

		LOG(Display, "Interstitial Load Attempt");

		return true;
	}

	return false;
}

bool UAdsManager::LoadRewarded()
{
	if(IsRewardedLoaded()) return false;
	
	if(UAppLovinProxy* Proxy = UAppLovinProxy::GetApplovin())
	{
		Proxy->LoadRewardedVideo(GetRewardedPlacement());

		LOG(Display, "Rewarded Load Attempt");

		return true;
	}

	return false;
}

bool UAdsManager::ShowRewarded()
{
	if(!IsRewardedLoaded())
	{
		LoadRewarded();

		return false;
	}
	
	if(UAppLovinProxy* Proxy = UAppLovinProxy::GetApplovin())
	{
		Proxy->ShowRewardedVideo(GetRewardedPlacement());

		LOG(Display, "Rewarded Show Attempt");

		return true;
	}

	return false;
}

bool UAdsManager::ShowInterstitial()
{
	if(!IsAdsEnabled()) return false;
	
	if(!IsInterstitialLoaded())
	{
		LoadInterstitial();

		return false;
	}
	
	if(UAppLovinProxy* Proxy = UAppLovinProxy::GetApplovin())
	{
		Proxy->ShowInterstitial(GetInterstitialPlacement());

		LOG(Display, "Interstitial Show Attempt");

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
			for(const TSoftClassPtr<UShopItem>& SoftItemClass : AdsSystemSettings->NoAdsShopItemClasses)
			{
				if(SoftItemClass.LoadSynchronous() == Purchase.ShopItemClass || Purchase.ShopItemClass->IsChildOf(SoftItemClass.LoadSynchronous()))
				{
					return false;
				}
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

void UAdsManager::StartLoadAds()
{
	LOG(Display, "Startup Ads Load Attempt");
	
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

			LOG(Display, "Rewarded Closed");
			
			break;
		}
	case EAppLovinRewardedVideoEventType::Showed:
		{
			bRewardedLoaded = false;

			LOG(Display, "Rewarded Showed");
			
			break;
		}
	case EAppLovinRewardedVideoEventType::Loaded:
		{
			bRewardedLoaded = true;

			LOG(Display, "Rewarded Loaded");
			
			break;
		}
	case EAppLovinRewardedVideoEventType::Rewarded:
		{
			OnRewardedRewarded.Broadcast();

			LoadRewarded();

			LOG(Display, "Rewarded Rewarded");
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

			LOG(Display, "Interstitial Closed");

			OnInterstitialClosed.Broadcast();
			
			break;
		}
	case EAppLovinInterstitialEventType::Showed:
		{
			bInterstitialLoaded = false;

			LOG(Display, "Interstitial Showed");
			break;
		}
	case EAppLovinInterstitialEventType::Loaded:
		{
			bInterstitialLoaded = true;

			LOG(Display, "Interstitial Loaded");
			
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
	switch(EventType)
	{
	case EAppLovinRewardedErrorEventType::FailedToShow:
		{
			bRewardedLoaded = false;
			
			OnRewardedShowFailed.Broadcast(Code);

			LoadRewarded();

			LOG(Display, "Rewarded Failed To Show");
			
			break;
		}
	case EAppLovinRewardedErrorEventType::FailedToLoad:
		{
			LOG(Display, "Rewarded Failed To Load");
			
			if(!GetWorld()) break;
			
			if(const UAdsSystemSettings* AdsSystemSettings = GetDefault<UAdsSystemSettings>())
			{
				FTimerHandle RetryTimer;
				GetWorld()->GetTimerManager().SetTimer(RetryTimer,
					this,
					&UAdsManager::OnRewardedLoadRetry,
					AdsSystemSettings->AdsLoadRetryDelay
				);
			}

			break;
		}
	default:
		{
			break;
		}
	}
}

void UAdsManager::OnInterstitialError(EAppLovinInterstitialErrorEventType EventType, int Code, FString Message)
{
	switch(EventType)
	{
	case EAppLovinInterstitialErrorEventType::FailedToLoad:
		{
			LOG(Display, "Interstitial Failed To Load");
			
			if(!GetWorld()) break;
			
			if(const UAdsSystemSettings* AdsSystemSettings = GetDefault<UAdsSystemSettings>())
			{
				FTimerHandle RetryTimer;
				GetWorld()->GetTimerManager().SetTimer(RetryTimer,
					this,
					&UAdsManager::OnInterstitialLoadRetry,
					AdsSystemSettings->AdsLoadRetryDelay
				);
			}

			break;
		}
	case EAppLovinInterstitialErrorEventType::FailedToShow:
		{
			bInterstitialLoaded = false;
			
			LoadInterstitial();

			LOG(Display, "Interstitial Failed To Show");
			break;
		}
	default:
		{
			break;
		}
	}
}

void UAdsManager::OnRewardedLoadRetry()
{
	LOG(Display, "Rewarded Load Retry");
	
	if(!LoadRewarded())
	{
		if(!GetWorld()) return;
			
		if(const UAdsSystemSettings* AdsSystemSettings = GetDefault<UAdsSystemSettings>())
		{
			FTimerHandle RetryTimer;
			GetWorld()->GetTimerManager().SetTimer(RetryTimer,
				this,
				&UAdsManager::OnRewardedLoadRetry,
				AdsSystemSettings->AdsLoadRetryDelay
			);
		}
	}
}

void UAdsManager::OnInterstitialLoadRetry()
{
	LOG(Display, "Interstitial Load Retry");
	
	if(!LoadInterstitial())
	{
		if(!GetWorld()) return;
			
		if(const UAdsSystemSettings* AdsSystemSettings = GetDefault<UAdsSystemSettings>())
		{
			FTimerHandle RetryTimer;
			GetWorld()->GetTimerManager().SetTimer(RetryTimer,
				this,
				&UAdsManager::OnInterstitialLoadRetry,
				AdsSystemSettings->AdsLoadRetryDelay
			);
		}
	}
}

void UAdsManager::OnShopItemBought(UShopItem* ShopItem)
{
	if(!IsAdsEnabled())
	{
		const UManagersSystem* ManagersSystem = GetManagerSystem();
		if(!ManagersSystem) return;

		UShopManager* ShopManager = ManagersSystem->GetManager<UShopManager>();
		if(!ShopManager) return;

		ShopManager->OnItemBought.RemoveDynamic(this, &UAdsManager::OnShopItemBought);

		OnAdsDisabled.Broadcast(ShopItem);
	}
}

void UAdsManager::BindAppLovinEvents()
{
	UAppLovinProxy::GetApplovin()->OnRewardedVideoEvent.AddUniqueDynamic(this, &UAdsManager::OnRewarded);
	UAppLovinProxy::GetApplovin()->OnRewardedVideoErrorEvent.AddUniqueDynamic(this, &UAdsManager::OnRewardedError);

	if(IsAdsEnabled())
	{
		UAppLovinProxy::GetApplovin()->OnInterstitialEvent.AddUniqueDynamic(this, &UAdsManager::OnInterstitial);
		UAppLovinProxy::GetApplovin()->OnInterstitialErrorEvent.AddUniqueDynamic(this, &UAdsManager::OnInterstitialError);
	}
}

void UAdsManager::BindShopEvents()
{
	if(IsAdsEnabled())
	{
		const UManagersSystem* ManagersSystem = GetManagerSystem();
		if(!ManagersSystem) return;

		UShopManager* ShopManager = ManagersSystem->GetManager<UShopManager>();
		if(!ShopManager) return;

		ShopManager->OnItemBought.AddUniqueDynamic(this, &UAdsManager::OnShopItemBought);
	}
}
