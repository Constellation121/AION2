#include "pch.h"
#include "CoreGlobal.h"
#include "ThreadManager.h"
#include "JobQueue.h"
#include "JobTimer.h"
#include "SocketUtils.h"
#include "GlobalQueue.h"

ThreadManager* GThreadManager = nullptr;
GlobalQueue* GGlobalQueue = nullptr;
JobTimer* GJobTimer = nullptr;

class CoreGlobal
{
public:
	CoreGlobal()
	{
		GThreadManager = new ThreadManager();
		GGlobalQueue = new GlobalQueue();
		GJobTimer = new JobTimer();
		SocketUtils::Init();
	}

	~CoreGlobal()
	{
		delete GThreadManager;
		delete GGlobalQueue;
		delete GJobTimer;
		SocketUtils::Clear();
	}
} GCoreGlobal;
