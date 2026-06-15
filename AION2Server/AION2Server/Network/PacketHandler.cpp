#include "pch.h"
#include "PacketHandler.h"
#include "GameSession.h"
#include "DBConnectionPool.h"
#include "DBBind.h"

PacketHandlerFunc GPacketHandler[256];

bool PacketHandler::HandleSignUp(PacketSessionRef& session, C_SignUpPacket& pkt)
{
	cout << "SignUp Request: ID(" << pkt.id << ") PW(" << pkt.password << ") Class(" << static_cast<int32>(pkt.classType) << ")" << endl;
	DBConnection* dbConnect = GDBConnectionPool->Pop();
	DBBind<3, 1> dbBind(*dbConnect, L"{CALL sp_RegisterUser(?, ?, ?)}");

	// 패킷의 char[]을 WCHAR[]로 변환하기 위한 버퍼 선언
	WCHAR wId[51] = { 0, };
	WCHAR wPassword[51] = { 0, };

	// 안전한 복사를 위해 mbstowcs_s 사용
	::mbstowcs_s(nullptr, wId, 51, pkt.id, _TRUNCATE);
	::mbstowcs_s(nullptr, wPassword, 51, pkt.password, _TRUNCATE);

	// DBBind의 래퍼 함수를 이용해 파라미터 바인딩
	dbBind.BindParam(0, wId);
	dbBind.BindParam(1, wPassword);

	int32 classTypeInt = static_cast<int32>(pkt.classType);
	dbBind.BindParam(2, classTypeInt);

	// DB 실행
	int resultCode = 0;
	dbBind.BindCol(0, resultCode);

	if (dbBind.Execute())
	{
		if (dbBind.Fetch())
		{
			cout << "ResultCode : " << resultCode << endl;
		}
	}

	// DB 연결 반납
	GDBConnectionPool->Push(dbConnect);

	S_SignUpResultPacket sendPkt;
	sendPkt.header.packetType = EPacketType::S_SignUpResult;
	sendPkt.header.packetSize = sizeof(S_SignUpResultPacket);
	sendPkt.seccess = resultCode;
	SendBufferRef sendBuffer = PacketHandler::MakeSendBuffer(sendPkt, EPacketType::S_SignUpResult);
	session->Send(sendBuffer);
	return true;
}

bool PacketHandler::HandleLogin(PacketSessionRef& session, C_LoginPacket& pkt)
{
	cout << "Login Request: ID(" << pkt.id << ") PW(" << pkt.password << ")" << endl;
	DBConnection* dbConnect = GDBConnectionPool->Pop();
	DBBind<2, 1> dbBind(*dbConnect, L"{CALL sp_RegisterUser(?, ?)}");

	WCHAR wId[51] = { 0, };
	WCHAR wPassword[51] = { 0, };

	// 안전한 복사를 위해 mbstowcs_s 사용
	::mbstowcs_s(nullptr, wId, 51, pkt.id, _TRUNCATE);
	::mbstowcs_s(nullptr, wPassword, 51, pkt.password, _TRUNCATE);

	// DBBind의 래퍼 함수를 이용해 파라미터 바인딩
	dbBind.BindParam(0, wId);
	dbBind.BindParam(1, wPassword);
	int32 playerClass = 0;
	int32 exp = 0;
	int32 gold = 0;
	int32 hp = 0;
	int32 itemInstanceId = 0;
	int32 itemTemplateId = 0;
	int32 slotIndex = 0;
	int32 itemCount = 0;

	//  u.Class,
	//	u.Exp,
	//	u.Gold,
	//	u.Hp,
	//	i.ItemInstanceId,
	//	i.ItemTemplateId,
	//	i.SlotIndex,
	//	i.Count

	wcout.imbue(locale("kor"));

	if (dbBind.Execute())
	{
		while (dbBind.Fetch())
		{
			dbBind.BindCol(0, playerClass);
			dbBind.BindCol(1, exp);
			dbBind.BindCol(2, gold);
			dbBind.BindCol(3, hp);
			dbBind.BindCol(4, itemInstanceId);
			dbBind.BindCol(5, itemTemplateId);
			dbBind.BindCol(6, slotIndex);
			dbBind.BindCol(7, itemCount);
		}
	}

	GDBConnectionPool->Push(dbConnect);

	return true;
}
