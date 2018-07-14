#include "stdafx.h"
#include <thread>
#include "BiasedMutex.h"


BiasedMutex::BiasedMutex(int maxp)
{
	setMaxPriority(maxp);
}


BiasedMutex::BiasedMutex(BiasedMutex&& biasedmutex)
{
	maxPriority = biasedmutex.maxPriority;
	lockStates = std::move(biasedmutex.lockStates);
}


void BiasedMutex::setMaxAllowedBias(int maxbais)
{
	maxAllowedBias = maxbais;
}

void BiasedMutex::setMaxPriority(int maxp)
{
	maxPriority = maxp;
	for (int count = 0; count < maxp; count++)
	{
		lockStates.push_back(std::make_shared<LockState>());
		lockStates[count]->state = LOCKSTATE::FREE;
		lockStates[count]->waitCount = 0;
	}
	return;
}

void BiasedMutex::lock(int lockPriority, int reqPriority)
{
	int curVal = LOCKSTATE::FREE, writeVal = LOCKSTATE::LOCKED, waitCount = 0;
	
	// One of the thread with same priority will get to proceed
	if (lockPriority == reqPriority)
	{
		lockStates[lockPriority]->waitCount++;
		while ( !lockStates[lockPriority]->state.compare_exchange_weak(curVal, writeVal) )
		{
			std::this_thread::yield();
			curVal = LOCKSTATE::FREE;
		}
		lockStates[lockPriority]->waitCount--;
	}
	else
	{
		// When lower priorit thread compete with Higher priority thread,Higher priority thread will get the chance to proceed
		// To avoid startvation, Lower priority thread will promote to higher priority after max try
		while (((lockPriority > reqPriority) && lockStates[lockPriority]->waitCount != 0) || !lockStates[lockPriority]->state.compare_exchange_weak(curVal, writeVal))
		{
			std::this_thread::yield();
			if (lockPriority > reqPriority)
			{
				waitCount++;   // Low priority task should not suffer limitless
				if (waitCount > maxAllowedBias)   // After max generocity low priority task should be treated same like high priority task
					reqPriority = lockPriority;
			}
			curVal = LOCKSTATE::FREE;
		}
	}
	return;
}

void BiasedMutex::lock(int priority)
{
	for (int prio = 0; prio < maxPriority; prio++)
		if (prio >= priority - 1)
			lock(prio, priority - 1);
	return;
}

void BiasedMutex::unlock(int priority)
{
	for (int prio = priority - 1; prio < maxPriority; prio++)
		lockStates[prio]->state = LOCKSTATE::FREE;
	return;
}
