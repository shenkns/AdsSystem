// Copyright shenkns Ads System Developed With Unreal Engine. All Rights Reserved 2023.

#pragma once

#include "Items/ShopItem.h"

#include "ShopItemAds.generated.h"

UCLASS()
class ADSSYSTEM_API UShopItemAds : public UShopItem
{
	GENERATED_BODY()

public:

	virtual void Init_Implementation() override;

	virtual bool CanBeBought_Implementation() const override;

	virtual bool Buy_Implementation() override;

protected:

	UFUNCTION(BlueprintCallable, Category = "Shop|Ads")
	void ValidateAds();

private:

	void UnBindAllEvents();

	UFUNCTION()
	void FinishPurchaseOnRewarded();

	UFUNCTION()
	void FailPurchaseOnFailed(int Code);
};
