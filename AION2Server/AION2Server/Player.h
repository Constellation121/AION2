#pragma once

class Player
{
public:
	Player() = default;
	Player(int32 playerClass, int32 exp, int32 gold, int32 hp);
public:
	uint64 _playerId = 0;
	string _name;
	weak_ptr<class GameSession>_ownerSession;

private:
	int32 _class;
	int32 _gold;
	int32 _hp;
	int32 _exp;
};
