// Copyright shenkns Ads System Developed With Unreal Engine. All Rights Reserved 2023.

using UnrealBuildTool;

public class AdsSystem : ModuleRules
{
	public AdsSystem(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		CppStandard = CppStandardVersion.Cpp17;

		PrivateIncludePaths.AddRange(
			new string[] {
				"AdsSystem/Public/"
			}
		);

		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core"
			}
		);

		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine",
				"LogSystem",
				"ManagersSystem",
				"DataSystem",
				"ShopSystem",
				"AppLovin",
				"SaveLoadSystem",
				"SerializationSystem"
			}
		);
		
		PrivateIncludePathModuleNames.AddRange(
			new string[]
			{
				"LogSystem",
				"ManagersSystem",
				"DataSystem",
				"ShopSystem",
				"AppLovin",
				"SaveLoadSystem",
				"SerializationSystem"
			}
		);
	}
}
