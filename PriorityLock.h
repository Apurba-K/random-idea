#pragma once

#include "BiasedMutex.h"

class PriorityLock
{
	BiasedMutex& bm;
	int curReqPriority;

public:
	PriorityLock(BiasedMutex& bm_, int priority);
	~PriorityLock();
};
