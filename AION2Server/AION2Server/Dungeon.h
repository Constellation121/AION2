#pragma once
#include <map>
#include "Room.h"

const int MAX_DUNGEON = 4;
const int MAX_MEMBERS = 3;

class Dungeon : public Room
{
public:
	virtual ~Dungeon() override;
	//void AddDungeon(Protocol::DungeonInfo member);
	void AddWaitingRoom(PlayerRef player);
	void HandleWaitingRoom(PlayerRef player);

	void HandleCreateDungeon(PlayerRef player);

	void HandleEnterDungeon(PlayerRef player);
	int32 GetFreeIndex(Protocol::DungeonInfo& Dungeon);

	int32 GetFreeDungeonId();

	void WaitingRoomBroadcast(SendBufferRef sendBuffer, uint64 exceptId);

private:
	// 던전 정보(던전 아이디: 던전 정보)
	std::map<int32, Protocol::DungeonInfo> _dungeons;
	std::map<int32, DedicatedSessionRef> _dungeonDediSessions;
	std::set<int32> _freeDungeonIds;
	uint32_t _nextRoomId = 1;

	// 대기방에 있는 플레이어들 
	std::map<uint64, PlayerRef> _waitingPlayers;
};

extern DungeonRef GDungeon;
