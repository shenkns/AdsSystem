// Copyright shenkns Ads System Developed With Unreal Engine. All Rights Reserved 2023.

#include "Managers/AdsManager.h"

#include "LogSystem.h"
#include "AppLovinProxy.h"
#include "ManagersSystem.h"
#include "Managers/StatsManager.h"
#include "Data/ShopItemData.h"
#include "Kismet/GameplayStatics.h"
#include "Module/AdsSystemModule.h"
#include "Module/AdsSystemSettings.h"
#include "Stats/StatShopHistory.h"

void UAdsManager::InitManager()
{
	Super::InitManager();

	if(!UAppLovinProxy::GetApplovin()) return;
	UAppLovinProxy::GetApplovin()->Init();

	LOG(LogAdsSystem, """%s"" AppLovin Proxy Initialized", *UAppLovinProxy::GetApplovin()->GetName())

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
	if(!IsAdsEnabled() || IsInterstitialLoaded()) return false;
	
	if(UAppLovinProxy* Proxy = UAppLovinProxy::GetApplovin())
	{
		Proxy->LoadInterstitial(GetInterstitialPlacement());

		LOG(LogAdsSystem, "Interstitial Load Attempt")

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

		LOG(LogAdsSystem, "Rewarded Load Attempt")

		return true;
	}

	return false;
}

bool UAdsManager::ShowRewarded()
{
	if(!IsRewardedLoaded()) return false;
	
	if(UAppLovinProxy* Proxy = UAppLovinProxy::GetApplovin())
	{
		Proxy->ShowRewardedVideo(GetRewardedPlacement());

		LOG(LogAdsSystem, "Rewarded Show Attempt")

		return true;
	}

	return false;
}

bool UAdsManager::ShowInterstitial()
{
	if(!IsInterstitialLoaded()) return false;
	
	if(!IsAdsEnabled()) return false;
	
	if(UAppLovinProxy* Proxy = UAppLovinProxy::GetApplovin())
	{
		Proxy->ShowInterstitial(GetInterstitialPlacement());

		LOG(LogAdsSystem, "Interstitial Show Attempt")

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

void UAdsManager::StartLoadLoadAds()
{
	LOG(LogAdsSystem, "Startup Ads Load Attempt")
	
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

			LOG(LogAdsSystem, "Rewarded Closed")
			
			break;
		}
	case EAppLovinRewardedVideoEventType::Showed:
		{
			bRewardedLoaded = false;

			LOG(LogAdsSystem, "Rewarded Showed")
			
			break;
		}
	case EAppLovinRewardedVideoEventType::Loaded:
		{
			bRewardedLoaded = true;

			LOG(LogAdsSystem, "Rewarded Loaded")
			
			break;
		}
	case EAppLovinRewardedVideoEventType::Rewarded:
		{
			OnRewardedRewarded.Broadcast();

			LoadRewarded();

			LOG(LogAdsSystem, "Rewarded Rewarded")
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

			LOG(LogAdsSystem, "Interstitial Closed")
			
			break;
		}
	case EAppLovinInterstitialEventType::Showed:
		{
			bInterstitialLoaded = false;

			LOG(LogAdsSystem, "Interstitial Showed")
			break;
		}
	case EAppLovinInterstitialEventType::Loaded:
		{
			bInterstitialLoaded = true;

			LOG(LogAdsSystem, "Interstitial Loaded")
			
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

			LOG(LogAdsSystem, "Rewarded Failed To Show")
			
			break;
		}
	case EAppLovinRewardedErrorEventType::FailedToLoad:
		{
			LOG(LogAdsSystem, "Rewarded Failed To Load")
			
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
			LOG(LogAdsSystem, "Interstitial Failed To Load")
			
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

			LOG(LogAdsSystem, "Interstitial Failed To Show")
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
	LOG(LogAdsSystem, "Rewarded Load Retry")
	
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
	LOG(LogAdsSystem, "Interstitial Load Retry")
	
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
