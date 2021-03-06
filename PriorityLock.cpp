// PriorityLock.cpp : Defines the entry point for the console application.

#include "stdafx.h"
#include "PriorityLock.h"

PriorityLock::PriorityLock(BiasedMutex& bm_, int priority) : bm( bm_ ), curReqPriority(priority)
{
	bm.lock(priority);
}

PriorityLock::~PriorityLock()
{
	bm.unlock(curReqPriority);
}



