// Copyright shenkns Ads System Developed With Unreal Engine. All Rights Reserved 2023.

#pragma once

#include "Items/ShopItem.h"

#include "ShopItemAds.generated.h"

UCLASS()
class ADSSYSTEM_API UShopItemAds : public UShopItem
{
	GENERATED_BODY()

public:

	virtual bool CanBeBought_Implementation() const override;

	virtual void Buy_Implementation() override;

private:

	void UnBindAllEvents();

	UFUNCTION()
	void FinishPurchaseOnRewarded();

	UFUNCTION()
	void FailPurchaseOnFailed(int Code);
};
