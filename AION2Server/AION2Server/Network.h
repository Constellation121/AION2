#pragma once

class Network
{
public:
	void Init();
	bool BindListen();

private:
	SOCKET _listenSocket;
	SOCKET _clientSocket;

	AcceptEvent _acceptEvent;

	HANDLE _iocpHandle;
};

