#pragma once

class Player
{
public:
	Player() = default;
	Player(int32 playerClass, int32 exp, int32 gold, int32 hp);

	void SetPlayerInfo(int32 playerClass, int32 exp, int32 gold, int32 hp);

public:
	uint64 _playerId = 0;
	std::string _name;
	std::weak_ptr<class GameSession>_ownerSession;

public:
	int32 _class;
	int32 _gold;
	int32 _hp;
	int32 _exp;
};
