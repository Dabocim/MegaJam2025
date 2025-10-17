// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class MegaJam : ModuleRules
{
	public MegaJam(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] {
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"EnhancedInput",
			"AIModule",
			"StateTreeModule",
			"GameplayStateTreeModule",
			"UMG",
			"Slate"
		});

		PrivateDependencyModuleNames.AddRange(new string[] { });

		PublicIncludePaths.AddRange(new string[] {
			"MegaJam",
			"MegaJam/Variant_Platforming",
			"MegaJam/Variant_Platforming/Animation",
			"MegaJam/Variant_Combat",
			"MegaJam/Variant_Combat/AI",
			"MegaJam/Variant_Combat/Animation",
			"MegaJam/Variant_Combat/Gameplay",
			"MegaJam/Variant_Combat/Interfaces",
			"MegaJam/Variant_Combat/UI",
			"MegaJam/Variant_SideScrolling",
			"MegaJam/Variant_SideScrolling/AI",
			"MegaJam/Variant_SideScrolling/Gameplay",
			"MegaJam/Variant_SideScrolling/Interfaces",
			"MegaJam/Variant_SideScrolling/UI"
		});

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
