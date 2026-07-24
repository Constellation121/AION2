// Copyright Epic Games, Inc. All Rights Reserved.

using System.IO;
using UnrealBuildTool;

public class AION2 : ModuleRules
{
    public AION2(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        IWYUSupport = IWYUSupport.Full;
        bUseUnity = false;
        bUseRTTI = true;
        bEnableExceptions = true;

        PublicIncludePaths.AddRange(new string[] { "AION2" });

        // The original project used a private ProtobufCore module which is not
        // part of this checkout. Use Unreal Engine's bundled Protobuf runtime.
        PrivateDependencyModuleNames.Add("Protobuf");

        // UE 5.6's Protobuf 3.18 package is missing these two generated-code
        // support includes, so keep the compatibility copies local to AION2.
        PrivateIncludePaths.Add(Path.Combine(ModuleDirectory, "ThirdParty", "ProtobufCompat"));

        PublicDependencyModuleNames.AddRange(new string[]
        {
            "Core",
            "CoreUObject",
            "Engine",
            "InputCore",
            "EnhancedInput",
            "Networking",
            "Sockets",
            "GameplayAbilities",
            "GameplayTags",
            "GameplayTasks",
            "AIModule",
            "StateTreeModule",
            "GameplayStateTreeModule",
            "Niagara",
            "UMG",
            "NavigationSystem",
            "Navmesh",
            "HTTP",
            "MotionWarping",
            "StateTreeModule",
            "GameplayStateTreeModule"
        });

        PrivateDependencyModuleNames.AddRange(new string[] {
            "Slate",
            "SlateCore"
        });

        CppCompileWarningSettings.UndefinedIdentifierWarningLevel = WarningLevel.Off;
        CppCompileWarningSettings.ShadowVariableWarningLevel = WarningLevel.Off;

        // PrivateDependencyModuleNames.Add("OnlineSubsystem");
        // To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
    }
}
