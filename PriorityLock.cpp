// PriorityLock.cpp : Defines the entry point for the console application.

#include "stdafx.h"
#include "PriorityLock.h"

PriorityLock::PriorityLock(BiasedMutex& bm_, BiasedMutex::BIAS bias_) : bm( bm_ ), bias(bias_)
{
	bm.lock(bias);
}

PriorityLock::~PriorityLock()
{
	bm.unlock(bias);
}



