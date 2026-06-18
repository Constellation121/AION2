// Copyright Epic Games, Inc. All Rights Reserved.

using System.IO;
using UnrealBuildTool;

public class AION2 : ModuleRules
{
    public AION2(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        string ProjectRoot = Target.ProjectFile.Directory.ToString();
        string ProtobufPath = Path.Combine(ProjectRoot, "Protobuf");
        string ProtobufIncludePath = Path.Combine(ProtobufPath, "include");
        string ProtobufLibPath = Path.Combine(ProtobufPath, "lib");

        PublicSystemIncludePaths.Add(ProtobufIncludePath);
        PublicIncludePaths.Add(ProtobufIncludePath);
        PublicAdditionalLibraries.Add(Path.Combine(ProtobufLibPath, "libprotobuf.lib"));
        if (Directory.Exists(ProtobufLibPath))
        {
            string[] LibFiles = Directory.GetFiles(ProtobufLibPath, "*.lib");
            foreach (string LibFile in LibFiles)
            {
                PublicAdditionalLibraries.Add(LibFile);
            }
        }

        PublicIncludePaths.AddRange(new string[] { "AION2" });
        PublicDefinitions.AddRange(
           new string[]
           {
                "GOOGLE_PROTOBUF_NO_RTTI=1",
                "ABSL_FLAGS_STRIP_NAMES=1"
           });

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
            "UMG"
        });

        PrivateDependencyModuleNames.AddRange(new string[] {
            "Slate",
            "SlateCore"
        });


        if (Target.Platform == UnrealTargetPlatform.Win64)
        {
            // 1. 소스 코드가 있는 Common 폴더 경로 지정
            string CommonBinariesDir = Path.Combine(ProjectRoot, "Common", "bin");

            // 2. 최종 빌드 결과물이 나가는 Binaries/Win64 폴더 경로 지정
            string TargetBinariesDir = Path.Combine(ModuleDirectory, "..", "..", "Binaries", "Win64");
            // 2. Common 폴더가 실제로 존재할 때만 실행
            if (Directory.Exists(CommonBinariesDir))
            {
                // 3. Common 폴더 내의 모든 *.dll 파일 목록을 자동으로 가져옴
                string[] DllFiles = Directory.GetFiles(CommonBinariesDir, "*.dll");

                foreach (string SrcDllPath in DllFiles)
                {
                    // 파일명만 추출 (예: "libprotobuf.dll")
                    string DllName = Path.GetFileName(SrcDllPath);
                    // 최종적으로 복사될 목적지 경로 생성
                    string TgtDllPath = Path.Combine(TargetBinariesDir, DllName);

                    // 4. 언리얼 빌드 시스템에 런타임 의존성 등록 (매우 중요)
                    RuntimeDependencies.Add(TgtDllPath, SrcDllPath);

                    // 5. 빌드 시점에 목적지 폴더가 없으면 생성하고 복사
                    if (!Directory.Exists(TargetBinariesDir))
                    {
                        Directory.CreateDirectory(TargetBinariesDir);
                    }

                    // 파일이 없거나, 소스 파일이 더 최신 파일일 때만 복사 수행
                    if (!File.Exists(TgtDllPath) || File.GetLastWriteTime(SrcDllPath) != File.GetLastWriteTime(TgtDllPath))
                    {
                        File.Copy(SrcDllPath, TgtDllPath, true);
                    }
                }
            }
        }

        // Uncomment if you are using online features
        // PrivateDependencyModuleNames.Add("OnlineSubsystem");

        // To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
    }
}
