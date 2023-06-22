// Copyright shenkns Ads System Developed With Unreal Engine. All Rights Reserved 2023.

#pragma once

#include "AppLovinProxy.h"
#include "Managers/Manager.h"

#include "AdsManager.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FRewardedRewardEvent);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FRewardedShowFailEvent, int, Code);

UCLASS()
class ADSSYSTEM_API UAdsManager : public UManager
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintAssignable, Category = "Ads")
	FRewardedRewardEvent OnRewardedRewarded;

	UPROPERTY(BlueprintAssignable, Category = "Ads")
	FRewardedShowFailEvent OnRewardedShowFailed;

protected:

	bool bRewardedLoaded = false;
	bool bInterstitialLoaded = false;

public:

	virtual void InitManager() override;

	UFUNCTION(BlueprintCallable, Category = "Ads")
	void StartLoadLoadAds();

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

	UFUNCTION(BlueprintPure, Category = "Ads")
	bool IsRewardedLoaded() const { return bRewardedLoaded; }

	UFUNCTION(BlueprintPure, Category = "Ads")
	bool IsInterstitialLoaded() const { return bInterstitialLoaded; }

private:

	UFUNCTION()
	void OnRewarded(EAppLovinRewardedVideoEventType EventType);

	UFUNCTION()
	void OnInterstitial(EAppLovinInterstitialEventType EventType);

	UFUNCTION()
	void OnRewardedError(EAppLovinRewardedErrorEventType EventType, int Code, FString Message);

	UFUNCTION()
	void OnInterstitialError(EAppLovinInterstitialErrorEventType EventType, int Code, FString Message);

	UFUNCTION()
	void OnRewardedLoadRetry();

	UFUNCTION()
	void OnInterstitialLoadRetry();
};
