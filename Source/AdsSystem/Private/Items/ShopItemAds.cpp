// Copyright shenkns Ads System Developed With Unreal Engine. All Rights Reserved 2023.

#include "Items/ShopItemAds.h"

#include "ManagersSystem.h"
#include "Managers/AdsManager.h"

bool UShopItemAds::CanBeBought_Implementation() const
{
	const UManagersSystem* ManagersSystem = GetManagersSystem();
	if(!ManagersSystem) return false;

	const UAdsManager* AdsManager = ManagersSystem->GetManager<UAdsManager>();
	if(!AdsManager) return false;

	return AdsManager->IsRewardedLoaded();
}

void UShopItemAds::Buy_Implementation()
{
	const UManagersSystem* ManagersSystem = GetManagersSystem();
	if(!ManagersSystem) return;

	UAdsManager* AdsManager = ManagersSystem->GetManager<UAdsManager>();
	if(!AdsManager) return;

	AdsManager->OnRewardedRewarded.AddUniqueDynamic(this, &UShopItemAds::FinishPurchaseOnRewarded);
	AdsManager->OnRewardedShowFailed.AddUniqueDynamic(this, &UShopItemAds::FailPurchaseOnFailed);

	AdsManager->ShowRewarded();
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

	FinishPurchase(true);
}

void UShopItemAds::FailPurchaseOnFailed(int Code)
{
	UnBindAllEvents();

	FinishPurchase(false);
}
