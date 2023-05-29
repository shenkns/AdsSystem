// Copyright shenkns Ads System Developed With Unreal Engine. All Rights Reserved 2023.

#pragma once

#include "AppLovinProxy.h"
#include "Managers/Manager.h"

#include "AdsManager.generated.h"

UCLASS()
class ADSSYSTEM_API UAdsManager : public UManager
{
	GENERATED_BODY()

public:

	virtual void InitManager() override;

	UFUNCTION(BlueprintPure, Category = "Ads")
	FString GetRewardedPlacement() const;

	UFUNCTION(BlueprintPure, Category = "Ads")
	FString GetInterstitialPlacement() const;

	UFUNCTION(BlueprintCallable, Category = "Ads")
	bool LoadInterstitial();

	UFUNCTION(BlueprintCallable, Category = "Ads")
	bool LoadRewarded();

	UFUNCTION(BlueprintCallable, Category = "Ads")
	bool ShowRewarded();
	
	UFUNCTION(BlueprintCallable, Category = "Ads")
	bool ShowInterstitial();

	UFUNCTION(BlueprintPure, Category = "Ads")
	bool IsAdsEnabled() const;

private:

	void StartLoadLoadAds();

	UFUNCTION()
	void OnRewarded(EAppLovinRewardedVideoEventType EventType);

	UFUNCTION()
	void OnInterstitial(EAppLovinInterstitialEventType EventType);

	UFUNCTION()
	void OnRewardedError(EAppLovinRewardedErrorEventType EventType, int Code, FString Message);

	UFUNCTION()
	void OnInterstitialError(EAppLovinInterstitialErrorEventType EventType, int Code, FString Message);
};
