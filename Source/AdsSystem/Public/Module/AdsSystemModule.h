// Copyright shenkns Ads System Developed With Unreal Engine. All Rights Reserved 2023.

#pragma once

#include "Modules/ModuleManager.h"

ADSSYSTEM_API DECLARE_LOG_CATEGORY_EXTERN(LogAdsSystem, Log, All);

class FAdsSystemModule : public IModuleInterface
{
public:

	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
	
private:

#if UE_EDITOR
	void RegisterSystemSettings() const;
	void UnregisterSystemSettings() const;
#endif
};
