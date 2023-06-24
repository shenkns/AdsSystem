// Copyright shenkns Ads System Developed With Unreal Engine. All Rights Reserved 2023.

#pragma once

#include "Data/CustomData.h"

#include "AdsShopCustomData.generated.h"

UCLASS()
class ADSSYSTEM_API UAdsShopCustomData : public UCustomData
{
	GENERATED_BODY()
	
public:
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Shop|Ads")
	bool bFreeWithDisabledAds;
};