#include "pch.h"
#include "PacketHandler.h"
#include "GameSession.h"
#include "DBConnectionPool.h"
#include "DBBind.h"
#include "ItemData.h"
#include "Player.h"

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

	//S_SignUpResultPacket sendPkt;
	//sendPkt.header.packetType = EPacketType::S_SignUpResult;
	//sendPkt.header.packetSize = sizeof(S_SignUpResultPacket);
	//sendPkt.success = resultCode;
	//SendBufferRef sendBuffer = PacketHandler::MakeSendBuffer(sendPkt, EPacketType::S_SignUpResult);
	//session->Send(sendBuffer);


		return true;
}

bool PacketHandler::HandleLogin(PacketSessionRef& session, C_LoginPacket& pkt)
{
	cout << "Login Request: ID(" << pkt.id << ") PW(" << pkt.password << ")" << endl;
	DBConnection* dbConnect = GDBConnectionPool->Pop();
	DBBind<2, 9> dbBind(*dbConnect, L"{CALL sp_LogIn(?, ?)}");

	WCHAR wId[51] = { 0, };
	WCHAR wPassword[51] = { 0, };

	// 안전한 복사를 위해 mbstowcs_s 사용
	::mbstowcs_s(nullptr, wId, 51, pkt.id, _TRUNCATE);
	::mbstowcs_s(nullptr, wPassword, 51, pkt.password, _TRUNCATE);

	// DBBind의 래퍼 함수를 이용해 파라미터 바인딩
	dbBind.BindParam(0, wId);
	dbBind.BindParam(1, wPassword);

	int32 errorCode = -1;
	int32 playerClass = 0;
	int32 exp = 0;
	int32 gold = 0;
	int32 hp = 0;
	int32 itemInstanceId = 0;
	int32 itemTemplateId = 0;
	int32 slotIndex = 0;
	int32 itemCount = 0;

	wcout.imbue(locale("kor"));
	dbBind.BindCol(0, errorCode);
	dbBind.BindCol(1, playerClass);
	dbBind.BindCol(2, exp);
	dbBind.BindCol(3, gold);
	dbBind.BindCol(4, hp);
	dbBind.BindCol(5, itemInstanceId);
	dbBind.BindCol(6, itemTemplateId);
	dbBind.BindCol(7, slotIndex);
	dbBind.BindCol(8, itemCount);

	std::vector<ItemData> itemInfos;
	bool isFirstRow = true;
	bool loginSuccess = false;

	if (dbBind.Execute())
	{
		while (dbBind.Fetch())
		{
			if (errorCode == 1)
			{
				cout << "로그인 실패: 아이디 또는 비밀번호 불일치" << endl;
				break;
			}
			loginSuccess = true;
			if (isFirstRow)
			{
				PlayerRef player = std::make_shared<Player>(playerClass, exp, gold, hp);
				GameSessionRef gameSession = static_pointer_cast<GameSession>(session);
				gameSession->_currentPlayer = player;
				isFirstRow = false;
			}
			if (itemInstanceId != 0)
			{
				ItemData item;
				item._itemInstanceId = itemInstanceId;
				item._itemTemplateId = itemTemplateId;
				item._slotIndex = slotIndex;
				item._count = itemCount;
				itemInfos.emplace_back(item);
			}

		}
	}

	GDBConnectionPool->Push(dbConnect);

	if (loginSuccess)
	{
		// TODO: 수집 완료된 inventory 벡터를 플레이어 객체에 주입
		// session->GetPlayer()->GetInventory()->InitItems(inventory);
		cout << "로그인 성공! 로드된 아이템 개수: " << itemInfos.size() << endl;
		S_LoginSuccesePacket sendPkt;
		sendPkt.header.packetType = EPacketType::S_LoginResult;
		sendPkt.header.packetSize = sizeof(S_LoginSuccesePacket);
		sendPkt.success = loginSuccess;
		SendBufferRef sendBuffer = PacketHandler::MakeSendBuffer(sendPkt, EPacketType::S_LoginResult);
		session->Send(sendBuffer);

		return true;
	}

	return false;
}
