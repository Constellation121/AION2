// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/MyNavExporter.h"
#include "NavigationSystem.h"
#include "NavMesh/RecastNavMesh.h"
#include "Misc//FileHelper.h"
#include "Detour/DetourNavMesh.h"
#include "Engine/World.h"

UMyNavExporter::UMyNavExporter()
{
}

UMyNavExporter::~UMyNavExporter()
{
}

void UMyNavExporter::ExportNavMesh(UWorld* World)
{
	if (!World) return;

	UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(World);
	if (!NavSys) return;

	ARecastNavMesh* RecastNavData = Cast<ARecastNavMesh>(NavSys->GetMainNavData());
	if (!RecastNavData) return;

	const dtNavMesh* DetourMesh = RecastNavData->GetRecastMesh();
	if (!DetourMesh) return;

	TArray<uint8> BinaryData;
	FMemoryWriter Writer(BinaryData);

	// 헤더 정보 저장 (서버가 타일 개수 등을 미리 알 수 있도록)
	int32 MaxTiles = DetourMesh->getMaxTiles();
	Writer << MaxTiles;

	// dtNavMeshParams 저장 (서버 dtNavMesh 초기화에 필수)
	const dtNavMeshParams* Params = DetourMesh->getParams();
	Writer.Serialize((void*)Params, sizeof(dtNavMeshParams));

	// 각 타일의 바이너리 데이터 순회 및 저장
	int32 ZeroSize = 0;
	for (int i = 0; i < MaxTiles; ++i)
	{
		const dtMeshTile* Tile = DetourMesh->getTile(i);
		if (!Tile || !Tile->header || Tile->dataSize <= 0)
		{
			Writer << ZeroSize;
			continue;
		}
		int DataSize = Tile->dataSize;
		Writer << DataSize;

		Writer.Serialize(Tile->data, DataSize);
	}
	 FString SavePath = FPaths::ProjectDir()/TEXT("Common/Nav/NavMesh.nav");
		// 외부 파일로 저장 
		 bool bSuccess = FFileHelper::SaveArrayToFile(BinaryData, *SavePath);

	if (bSuccess)
	{
		UE_LOG(LogTemp, Log, TEXT("NavMesh Binary Export Success: %s"), *SavePath);
	}
}
