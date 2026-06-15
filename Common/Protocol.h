#pragma once

#pragma pack(push, 1)
struct Vector3
{
	float x;
	float y;
	float z;
};

enum class EClassType : uint8_t
{
	Assassin = 1,
	Cleric,
	Ranger,
	Templar,
};

enum class EPacketType : uint8_t
{
	C_SignUp = 1,
	S_SignUpResult,
	C_Login,
	S_LoginResult,
};

struct PacketHeader
{
	uint16_t packetSize;
	EPacketType packetType;
};

struct C_SignUpPacket
{
	PacketHeader header;
	char id[20];
	char password[20];
	EClassType classType;
};

struct S_SignUpResultPacket
{
	PacketHeader header;
	uint8 seccess;
};

struct C_LoginPacket
{
	PacketHeader header;
	char id[20];
	char password[20];
};

struct S_LoginSuccesePacket
{
	PacketHeader header;
	uint8 seccess;
	uint16_t playerId;
};



#pragma pack(pop)