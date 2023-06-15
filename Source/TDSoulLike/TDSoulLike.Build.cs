// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class TDSoulLike : ModuleRules
{
	public TDSoulLike(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new string[] { 
			"Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay", 
			"NavigationSystem", "AIModule", "Niagara", "EnhancedInput", 
			"GameplayAbilities", "GameplayTags", "GameplayTasks" 
		});
    }
}
