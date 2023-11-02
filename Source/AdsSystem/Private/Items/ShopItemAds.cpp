// Copyright shenkns Ads System Developed With Unreal Engine. All Rights Reserved 2023.

#include "Items/ShopItemAds.h"

#include "ManagersSystem.h"
#include "Data/ShopItemData.h"
#include "Data/AdsShopCustomData.h"
#include "Managers/AdsManager.h"

void UShopItemAds::Init_Implementation()
{
	Super::Init_Implementation();

	ValidateAds();
}

bool UShopItemAds::CanBeBought_Implementation() const
{
	const UManagersSystem* ManagersSystem = GetManagersSystem();
	if(!ManagersSystem) return false;

	const UAdsManager* AdsManager = ManagersSystem->GetManager<UAdsManager>();
	if(!AdsManager) return false;
	
	if(const UShopItemData* AdsShopData = GetShopData<UShopItemData>())
	{
		if(const UAdsShopCustomData* AdsShopCustomData = AdsShopData->GetCustomData<UAdsShopCustomData>())
		{
			return (AdsShopCustomData->bFreeWithDisabledAds && !AdsManager->IsAdsEnabled()) || AdsManager->IsRewardedLoaded();
		}
	
		return AdsManager->IsRewardedLoaded();
	}
	
	return false;
}

bool UShopItemAds::Buy_Implementation()
{
	const UManagersSystem* ManagersSystem = GetManagersSystem();
	if(!ManagersSystem) return false;

	UAdsManager* AdsManager = ManagersSystem->GetManager<UAdsManager>();
	if(!AdsManager) return false;

	if(const UShopItemData* AdsShopData = GetShopData<UShopItemData>())
    {
		if(const UAdsShopCustomData* AdsShopCustomData = AdsShopData->GetCustomData<UAdsShopCustomData>())
		{
			if(AdsShopCustomData->bFreeWithDisabledAds && !AdsManager->IsAdsEnabled())
			{
				VerifyPurchase("ads_disabled");
	
				return true;
			}
		}
	
		AdsManager->OnRewardedRewarded.AddUniqueDynamic(this, &UShopItemAds::FinishPurchaseOnRewarded);
		AdsManager->OnRewardedShowFailed.AddUniqueDynamic(this, &UShopItemAds::FailPurchaseOnFailed);
		
		AdsManager->ShowRewarded();

		return true;
	}

	return false;
}

void UShopItemAds::ValidateAds()
{
	const UManagersSystem* ManagersSystem = GetManagersSystem();
	if(!ManagersSystem) return;

	UAdsManager* AdsManager = ManagersSystem->GetManager<UAdsManager>();
	if(!AdsManager) return;

	if(!AdsManager->IsRewardedLoaded())
	{
		AdsManager->LoadRewarded();
	}
}

void UShopItemAds::UnBindAllEvents()
{
	const UManagersSystem* ManagersSystem = GetManagersSystem();
	if(!ManagersSystem) return;

	UAdsManager* AdsManager = ManagersSystem->GetManager<UAdsManager>();
	if(!AdsManager) return;

	AdsManager->OnRewardedRewarded.RemoveDynamic(this, &UShopItemAds::FinishPurchaseOnRewarded);
	AdsManager->OnRewardedShowFailed.RemoveDynamic(this, &UShopItemAds::FailPurchaseOnFailed);
}

void UShopItemAds::FinishPurchaseOnRewarded()
{
	UnBindAllEvents();

	VerifyPurchase("ads_showed");
}

void UShopItemAds::FailPurchaseOnFailed(int Code)
{
	UnBindAllEvents();

	FinishPurchase(false);
}
