#pragma once
#include <atomic>
#include <vector>
#include <memory>

class BiasedMutex
{
	int maxAllowedBias = 4;                         // Max generocity to be shown by low priority job
	enum LOCKSTATE :int { LOCKED = -1, FREE };

	struct LockState
	{
		std::atomic< int > state;
		std::atomic< int > waitCount;
	};
		
	int maxPriority;
	std::vector< std::shared_ptr<LockState> >lockStates;

	void setMaxPriority(int maxp);
	void lock(int lockPriority, int reqPriority);

public:
	
	BiasedMutex(int maxp = 2);
	BiasedMutex(BiasedMutex&& biasedmutex);
	BiasedMutex(const BiasedMutex& biasedmutex) = delete;
	
	void lock(int priority);
	void unlock(int priority);

	void setMaxAllowedBias(int maxbais);
};
