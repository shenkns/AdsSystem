// Copyright shenkns Ads System Developed With Unreal Engine. All Rights Reserved 2023.

#include "Module/AdsSystemModule.h"

#if UE_EDITOR
#include "ISettingsModule.h"
#include "Module/AdsSystemSettings.h"
#endif

IMPLEMENT_MODULE(FAdsSystemModule, AdsSystem);

ADSSYSTEM_API DEFINE_LOG_CATEGORY(LogAdsSystem);

void FAdsSystemModule::StartupModule()
{
#if UE_EDITOR
	RegisterSystemSettings();
#endif
}

void FAdsSystemModule::ShutdownModule()
{
#if UE_EDITOR
	UnregisterSystemSettings();
#endif
}

#if UE_EDITOR
void FAdsSystemModule::RegisterSystemSettings() const
{
	if (ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
	{
		SettingsModule->RegisterSettings(
			"Project",
			"Plugins",
			"Ads System",
			FText::FromString(TEXT("Ads System")),
			FText::FromString(TEXT("Configuration settings for Ads System")),
			GetMutableDefault<UAdsSystemSettings>()
		);
	}
}

void FAdsSystemModule::UnregisterSystemSettings() const
{
	if (ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
	{
		SettingsModule->UnregisterSettings("Project", "Plugins", "Ads System");
	}
}
#endif
