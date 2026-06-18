#pragma once
#include "CoreMinimal.h"

enum : uint16
{
	PKT_C_SIGNUP = 1000,
	PKT_S_SIGNUP = 1001,
	PKT_C_LOGIN = 1002,
	PKT_S_SLOGIN = 1003,
	PKT_S_FLOGIN = 1004,
	PKT_S_ITEM = 1005,
};


struct FPacketHeader
{
	uint16 PacketSize;
	uint16 PacketId;
};