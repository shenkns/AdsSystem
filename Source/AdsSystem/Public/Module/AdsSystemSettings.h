// Copyright shenkns Ads System Developed With Unreal Engine. All Rights Reserved 2023.

#pragma once

#include "UObject/Object.h"

#include "AdsSystemSettings.generated.h"

UCLASS(Config=Game, DefaultConfig)
class ADSSYSTEM_API UAdsSystemSettings : public UObject
{
	GENERATED_BODY()

public:

	// Debug
	UPROPERTY(EditDefaultsOnly, Config, Category = "Debug")
	bool bShowDebugMessages;
};