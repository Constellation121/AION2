#pragma once

enum class EventType : uint8
{
	Accept,
	Recv,
	Send
};

class IocpEvent : public OVERLAPPED
{
public:
	IocpEvent(EventType type) : _type(type)
	{
		_wsaBuf.len = 0;
		_wsaBuf.buf = nullptr;

		::ZeroMemory(static_cast<OVERLAPPED*>(this), sizeof(OVERLAPPED));
	}

public:
	EventType	_type;
	WSABUF		_wsaBuf;
	uint64		_targetId;
};

class AcceptEvent : IocpEvent
{
public:
	AcceptEvent() : IocpEvent(EventType::Accept)
	{
		Init();
	}
	void Init()
	{
		::ZeroMemory(_acceptBuf, sizeof(_acceptBuf));
		_wsaBuf.len = sizeof(_acceptBuf);
		_wsaBuf.buf = _acceptBuf;
	}
public:
	char _acceptBuf[4096];
};

class RecvEvent : IocpEvent
{
public:
	RecvEvent() : IocpEvent(EventType::Recv)
	{
		_buffer.resize(4096);
		_wsaBuf.len = static_cast<ULONG>(_buffer.size());
		_wsaBuf.buf = _buffer.data();
	}
public:
	std::vector<char> _buffer;
};

class SendEvent : IocpEvent
{
	SendEvent() : IocpEvent(EventType::Send) {}

	void SendBuffer(const char* packet, size_t size)
	{
		_buffer.assign(packet, packet + size);
		_wsaBuf.len = static_cast<ULONG>(size);
		_wsaBuf.buf = _buffer.data();
	}
	std::vector<char> _buffer;
};