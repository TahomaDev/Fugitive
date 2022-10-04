// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Fugitive : ModuleRules
{
	public Fugitive(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
		
        PublicDependencyModuleNames.AddRange(new string[] { 
	        "Core",
	        "CoreUObject",
	        "Engine",
	        "InputCore",
	        "HeadMountedDisplay",
	        "NavigationSystem",
	        "AIModule",
	        "GameplayTasks",
	        "UMG",
	        "PhysicsCore",
	        "FMODStudio",
//	        ,"OnlineSubsystem"
        });
  /*      
        PrivateDependencyModuleNames.AddRange(new string[] { });
 
        PrivateDependencyModuleNames.Add("OnlineSubsystem");
        PrivateDependencyModuleNames.Add("OnlineSubsystemNull");*/
	}
}
