#include "pch.h"
#include "Network.h"

void Network::Init()
{
	WSADATA wsaData;
	if (::WSAStartup(MAKEWORD(2, 2), &wsaData))
	{
		return;
	}

	// 리슨 소켓 생성
	_listenSocket = ::WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);

	if (!BindListen)
	{
		return;
	}
	// iocp 핸들 생성
	_iocpHandle = ::CreateIoCompletionPort(INVALID_HANDLE_VALUE, nullptr, 0, 0);

	// iocp - 소켓 연결
	::CreateIoCompletionPort(reinterpret_cast<HANDLE>(_listenSocket), _iocpHandle, 9999, 0);

	// 연결할 클라이언트 주소를 받을 변수
	::SOCKADDR_IN clientAddr;
	int addrSize = sizeof(clientAddr);

	// 클라이언트 소켓 생성
	_clientSocket = ::WSASocket(AF_INET, SOCK_STREAM, 0, nullptr, 0, WSA_FLAG_OVERLAPPED);

	// accept
	
}

bool Network::BindListen()
{
	::SOCKADDR_IN serverAddr;
	serverAddr.sin_family = AF_INET;
	// 게임 포트 번호 클라이언트랑 맞추기
	serverAddr.sin_port = htons(9999);
	serverAddr.sin_addr.S_un.S_addr = INADDR_ANY;

	if (::bind(_listenSocket, reinterpret_cast<sockaddr*>(&serverAddr), sizeof(serverAddr)) == SOCKET_ERROR)
	{
		return false;
	}
	if (::listen(_listenSocket, SOMAXCONN) == SOCKET_ERROR)
	{
		return false;
	}
	return true;
}
