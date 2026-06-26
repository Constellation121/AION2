#include "pch.h"
#include "Dungeon.h"
#include "Player.h"
#include "PacketHandler.h"
#include "Protocol.pb.h"
#include "GameSession.h"
#include "DedicatedSession.h"
#include "DediSessionManager.h"


DungeonRef GDungeon = std::make_shared<Dungeon>();

Dungeon::~Dungeon()
{
}

void Dungeon::AddWaitingRoom(PlayerRef player)
{
	_waitingPlayers.emplace(player->GetId(), player);
}

void Dungeon::HandleWaitingRoom(PlayerRef player)
{
	AddWaitingRoom(player);

	// 새로 들어온 플레이어에게 던전 정보 전송

	Protocol::S_DungeonWaitingRoomEnterPacket waitPkt;
	for (const auto& [dungeonId, dungeon] : _dungeons)
	{
		Protocol::DungeonInfo* dungeonInfo = waitPkt.add_dungeoninfos();

		dungeonInfo->set_dungeonid(dungeonId);
		dungeonInfo->set_status(dungeon.status());

		Protocol::DungeonPlayerInfo* leaderInfo = dungeonInfo->mutable_leaderinfo();
		leaderInfo->set_membername(dungeon.leaderinfo().membername());
		leaderInfo->set_memberclass(dungeon.leaderinfo().memberclass());
		leaderInfo->set_isready(true);
		leaderInfo->set_index(0);

		const auto& members = dungeon.members();
		for (size_t i = 0; i < members.size(); ++i)
		{
			Protocol::DungeonPlayerInfo* memberInfos = dungeonInfo->add_members();
			memberInfos->set_membername(members[i].membername());
			memberInfos->set_memberclass(members[i].memberclass());
			memberInfos->set_isready(members[i].isready());
			memberInfos->set_index(members[i].index()); // index는 1, 2, 3으로 순차 부여 [cite: 24, 33]
		}
	}

	SendBufferRef DungeonBuffer = PacketHandler::MakeSendBuffer(waitPkt);
	if (auto session = player->_ownerSession.lock())
		session->Send(DungeonBuffer);
}

void Dungeon::HandleCreateDungeon(PlayerRef player)
{
	// 더이상 생성 못 한다는 패킷 보내기
	if (_dungeons.size() >= MAX_DUNGEON) return;

	DedicatedSessionRef dedi = GDediSessionManager.GetFreeDediSession();
	if (dedi == nullptr)
	{
		std::cout << " Full Dedi" << std::endl;
	}
	dedi->SetUsing(true);
	int32 dungeonId = GetFreeDungeonId();
	_dungeonDediSessions[dungeonId] = dedi;

	Protocol::DungeonInfo dungeon;
	{
		dungeon.set_dungeonid(dungeonId);
		dungeon.set_status(Protocol::RoomStatus::RECRUITING);

		Protocol::DungeonPlayerInfo* leader = dungeon.mutable_leaderinfo();
		leader->set_membername(player->GetName());
		leader->set_memberclass(player->GetClass());
		leader->set_index(0);
		leader->set_isready(true);

		_dungeons.emplace(dungeonId, dungeon);
	}

	// 기존 대기방 사람들에게 새로 만든 던전 정보 전송
	{
		Protocol::S_DungeonCreatePacket createPkt;
		Protocol::DungeonInfo* newDungeon = createPkt.mutable_dungeoninfo();
		newDungeon->set_dungeonid(dungeonId);
		newDungeon->set_status(dungeon.status());

		Protocol::DungeonPlayerInfo* leader = dungeon.mutable_leaderinfo();
		leader->set_membername(player->GetName());
		leader->set_memberclass(player->GetClass());
		leader->set_index(0);
		leader->set_isready(true);
		SendBufferRef createdungeonBuffer = PacketHandler::MakeSendBuffer(createPkt);

		WaitingRoomBroadcast(createdungeonBuffer, -1);
	}

}

void Dungeon::HandleEnterDungeon(PlayerRef player)
{
	int32 enterDungeonId = -1;
	Protocol::DungeonPlayerInfo* newPlayerInfo = nullptr;
	for (auto& [dungeonId, dungeon] : _dungeons)
	{
		if (dungeon.members_size() >= MAX_MEMBERS)
			continue;
		else
		{
			int32 index = GetFreeIndex(dungeon);
			if (index == -1)
				continue;

			enterDungeonId = dungeonId;
			newPlayerInfo = dungeon.add_members();;
			newPlayerInfo->set_membername(player->GetName());
			newPlayerInfo->set_memberclass(player->GetClass());
			newPlayerInfo->set_isready(false);
			newPlayerInfo->set_index(index);
			break;
		}
	}
	// Todo 실패 처리
	if (enterDungeonId == -1) return;

		// 입장한 던전 + 플레이어 정보를 대기방에 있는 모든 플레이어들에게 전송
	{
		Protocol::S_DungeonEnterPacket enterPacket;
		enterPacket.set_dungeonid(enterDungeonId);
		Protocol::DungeonPlayerInfo* enterPlayer = enterPacket.mutable_enterplayer();
		enterPlayer->CopyFrom(_dungeons[enterDungeonId]);

		SendBufferRef enterBuffer = PacketHandler::MakeSendBuffer(enterPacket);
		WaitingRoomBroadcast(enterBuffer, -1);
	}

}

int32 Dungeon::GetFreeIndex(Protocol::DungeonInfo& targetDungeon)
{
	for (int i = 0; i < MAX_MEMBERS; ++i)
	{
		bool isUsed = false;
		for (int32 j = 0; j < targetDungeon.members_size(); ++j)
		{
			if (targetDungeon.members(j).index() == i)
			{
				isUsed = true;
				break;
			}
		}
		if (!isUsed)
		{
			return i;
		}
	}
	return -1;
}

int32 Dungeon::GetFreeDungeonId()
{
	if (!_freeDungeonIds.empty())
	{
		auto it = _freeDungeonIds.begin();
		int32 id = *it;
		_freeDungeonIds.erase(it);
		return id;
	}
	return _nextRoomId++;
}
void Dungeon::WaitingRoomBroadcast(SendBufferRef sendBuffer, uint64 exceptId)
{
	for (auto& item : _waitingPlayers)
	{
		PlayerRef player = std::dynamic_pointer_cast<Player>(item.second);
		if (player == nullptr)
			continue;
		if (player->_playerId == exceptId)
			continue;
		if (GameSessionRef session = player->_ownerSession.lock())
		{
			session->Send(sendBuffer);
		}
	}
}
