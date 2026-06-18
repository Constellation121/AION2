#pragma once
#include "CoreMinimal.h"

enum : uint16
{
	PKT_C_SIGNUP = 1000,
	PKT_S_SIGNUP = 1001,
	PKT_C_LOGIN = 1002,
	PKT_S_LOGIN = 1003,
};

struct FPacketHeader
{
	uint16 PacketSize;
	uint16 PacketId;
};