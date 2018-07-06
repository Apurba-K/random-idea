#pragma once

#include "BiasedMutex.h"

class PriorityLock
{
	BiasedMutex& bm;
	BiasedMutex::BIAS bias;

public:
	PriorityLock(BiasedMutex& bm_, BiasedMutex::BIAS bias_);
	~PriorityLock();
};