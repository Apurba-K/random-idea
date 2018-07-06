#pragma once
#include <atomic>

class BiasedMutex
{
	int maxAllowedBias = 4;                         // Max generocity to be shown by low priority job
	enum LOCKSTATE :int { LOCKED = -1, FREE };
	
	std::atomic< int > lockState1, lockState2;      // Current state lockable
	std::atomic< int > priorityWaitCount;           // Depth of pending and continuing priority requests  at present
public:
	enum class BIAS :int { HIGHEST, LOW };          // Biased on which lock is required

	BiasedMutex();
	BiasedMutex(BiasedMutex&& biasedmutex);
	BiasedMutex(const BiasedMutex& biasedmutex);

	void lock(BIAS bias);
	void unlock(BIAS bias);

	void setMaxAllowedBias(int maxbais);
};