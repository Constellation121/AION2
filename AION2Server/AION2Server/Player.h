#pragma once

class Player
{
public:
	uint64 _playerId = 0;
	string _name;
	weak_ptr<class GameSession>_ownerSession;
};

