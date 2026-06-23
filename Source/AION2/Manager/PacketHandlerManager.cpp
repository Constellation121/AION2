// AOPacketHandler.cpp
#include "Manager/PacketHandlerManager.h"
#include "PacketHandler.h"
#include "Manager/AOPlayerManager.h"
#include "Game/AOGameInstance.h"
#include "AONetworkManager.h"

PacketHandlerFunc GAOPacketHandler[UINT16_MAX];

// 초기화 함수: 전역 배열에 패킷 ID와 변환 정책 함수를 바인딩함
void InitPacketHandler()
{
	for (int32 i = 1000; i < UINT16_MAX; i++)
		GAOPacketHandler[i] = &Handle_INVALID;

	// 템플릿을 활용해 자동으로 파싱 정책을 맵핑함
	GAOPacketHandler[PKT_S_SIGNUP] = [](UAONetworkManager* Mng, uint8* Buf, int32 Len) {return HandlePacketPolicy<Protocol::S_SignUpResultPacket>(Handle_S_SIGNUP, Mng, Buf, Len);};
	GAOPacketHandler[PKT_S_FLOGIN] = [](UAONetworkManager* Mng, uint8* Buf, int32 Len) {return HandlePacketPolicy<Protocol::S_LoginFailPacket>(Handle_S_FLOGIN, Mng, Buf, Len);};
	GAOPacketHandler[PKT_S_SLOGIN] = [](UAONetworkManager* Mng, uint8* Buf, int32 Len) {return HandlePacketPolicy<Protocol::S_LoginSuccessPacket>(Handle_S_SLOGIN, Mng, Buf, Len);};
	GAOPacketHandler[PKT_S_ITEM] = [](UAONetworkManager* Mng, uint8* Buf, int32 Len) {return HandlePacketPolicy<Protocol::S_ItemDataPacket>(Handle_S_ITEM, Mng, Buf, Len);};
	GAOPacketHandler[PKT_S_SPAWN] = [](UAONetworkManager* Mng, uint8* Buf, int32 Len) {return HandlePacketPolicy<Protocol::S_SpawnPacket>(Handle_S_SPAWN, Mng, Buf, Len);};
}

bool Handle_INVALID(UAONetworkManager* NetworkMng, uint8* Buffer, int32 Len)
{
	return false;
}

bool Handle_S_SIGNUP(UAONetworkManager* NetworkMng, Protocol::S_SignUpResultPacket& Pkt)
{
	return false;
}

bool Handle_S_SLOGIN(UAONetworkManager* NetworkMng, Protocol::S_LoginSuccessPacket& pkt)
{
	if (NetworkMng->PlayerMng && pkt.has_playerinfo())
	{
		uint64 PlayerId = pkt.playerinfo().playerid();
		uint8 ClassType = static_cast<uint8>(pkt.playerinfo().playerclass());

		NetworkMng->PlayerMng->HandleLogin(PlayerId, ClassType);
		NetworkMng->GameInstance->OnReadyoOpenLevel();
	}
	return true;
}

bool Handle_S_FLOGIN(UAONetworkManager* NetworkMng, Protocol::S_LoginFailPacket& Pkt)
{
	return false;
}

bool Handle_S_ITEM(UAONetworkManager* NetworkMng, Protocol::S_ItemDataPacket& Pkt)
{
	return false;
}

bool Handle_S_SPAWN(UAONetworkManager* NetworkMng, Protocol::S_SpawnPacket& Pkt)
{
	return false;
}
