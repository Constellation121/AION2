#pragma once
class PacketHandler
{
public:
	static void Init()
	{

	}

	static bool HandlePacket(PacketSessionRef& session, BYTE* buffer, int32 len)
	{

	}

private:
	template<typename PacketType, typename ProcessFunc>
	static bool HandlePacket(ProcessFunc Func, PacketSessionRef& session, BYTE* buffer, int32 len)
	{
		PacketType packet;
		if (packet.ParseFromArray(buffer + sizeof(PacketHeader), len - sizeof(PacketHeader)) == false)
			return false;
		return func(session, packet);
	}

	template<typename T>
	void MakeSendBuffer(T& packet, uint16 packetId)
	{
		const uint16 dataSize = static_cast<uint16>(packet.ByteSizeLong());
		const uint16 packetSize = dataSize + sizeof(PacketHeader);

		SendBufferRef sendBuffer = make_shared<SendBuffer>(packetSize);
	}
};

