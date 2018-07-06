#include "stdafx.h"
#include <thread>
#include "BiasedMutex.h"


BiasedMutex::BiasedMutex() : lockState1(LOCKSTATE::FREE), lockState2(LOCKSTATE::FREE), priorityWaitCount(0)
{}

BiasedMutex::BiasedMutex(BiasedMutex&& biasedmutex) : lockState1(static_cast<int>(biasedmutex.lockState1)),
                                                      lockState2(static_cast<int>(biasedmutex.lockState2)),
                                                      priorityWaitCount(static_cast<int>(biasedmutex.priorityWaitCount))
{}
BiasedMutex::BiasedMutex(const BiasedMutex& biasedmutex) : lockState1(LOCKSTATE::FREE),
                                                           lockState2(LOCKSTATE::FREE), priorityWaitCount(0)
{}

void BiasedMutex::setMaxAllowedBias(int maxbais)
{
	maxAllowedBias = maxbais;
}

void BiasedMutex::lock(BIAS bias)
{
	int curVal = LOCKSTATE::FREE, writeVal = LOCKSTATE::LOCKED, waitCountForLowBias = 0;
	bool marked = false;

	//A low priority request need to get primary lock
	//Because priorty work does not need to compete with all low priority work
	
	if (bias == BIAS::LOW)
	{
		while ( !lockState1.compare_exchange_weak(curVal, writeVal) )
		{
			std::this_thread::yield();
			curVal = LOCKSTATE::FREE;
		}
	}
	
	curVal = LOCKSTATE::FREE, writeVal = LOCKSTATE::LOCKED;
	// A request with low priority can be successful only when there is no pending high priority task

	while ( (bias == BIAS::LOW && priorityWaitCount != 0) || !lockState2.compare_exchange_weak(curVal, writeVal) )
	{
		if (bias == BIAS::HIGHEST && !marked)
		{
			// A high priority task is waiting for permission to proceed
			priorityWaitCount++;
			marked = true;
		}
		else if (bias == BIAS::LOW)
			waitCountForLowBias++;                 // Low priority task should not suffer limitless
		
		std::this_thread::yield();

		if (waitCountForLowBias > maxAllowedBias)   // After max generocity low priority task should be treated same like high priority task
			bias = BIAS::HIGHEST;
		
		curVal = LOCKSTATE::FREE;
	}

	if (marked)
		priorityWaitCount--;

	return;
}

void BiasedMutex::unlock(BIAS bias)
{
	if(bias == BIAS::HIGHEST)
		lockState2 = LOCKSTATE::FREE;
	else
	{
		lockState1 = lockState2 = LOCKSTATE::FREE;
	}
}