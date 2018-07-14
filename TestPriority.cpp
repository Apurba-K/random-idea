#include "stdafx.h"
#include "PriorityLock.h"

#include <string>
#include <iostream>
#include <thread>
#include <functional>
#include <chrono>
#include <map>

std::map< std::string, __int64 > durtionTable;

void priorityJob(std::string thName, int maxCount, BiasedMutex& bm)
{
	std::chrono::system_clock::time_point start = std::chrono::system_clock::now();
	for (int i = 0; i <= maxCount; ++i)
	{
		PriorityLock lock(bm, 2);
		//std::cout << "Priority Thread :" << thName << " processig:" << std::to_string(i) << "\n";
	}
	std::chrono::system_clock::time_point end = std::chrono::system_clock::now();
	durtionTable[thName] = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
	return;
}

void lowPriorityJob(std::string thName, int maxCount, BiasedMutex& bm)
{
	std::chrono::system_clock::time_point start = std::chrono::system_clock::now();
	for (int i = 0; i <= maxCount; ++i)
	{
		PriorityLock lock(bm, 1);
		//std::cout<< "Lowpriority Thread :"<< thName << " processig:" << std::to_string(i) << "\n";		
	}
	std::chrono::system_clock::time_point end = std::chrono::system_clock::now();
	durtionTable[thName] = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
	return;
}

int main()
{
	BiasedMutex bm;
	bm.setMaxAllowedBias(4);

	auto pjob = [&bm](std::string tname, int repeat) { priorityJob(tname, repeat, bm); };
	auto lowpJob = [&bm](std::string tname, int repeat) { lowPriorityJob(tname, repeat, bm); };

	std::thread lowprio1(lowpJob, "LowPriority1", 10000 );
	std::thread lowprio2(lowpJob, "LowPriority2", 10000 );
	std::thread lowprio3(lowpJob, "LowPriority3", 10000 );
	std::thread lowprio4(pjob, "LowPriority4", 10000 );
	std::thread lowprio5(lowpJob, "LowPriority5", 10000 );
	std::thread lowprio6(lowpJob, "LowPriority6", 10000 );
	std::thread lowprio7(lowpJob, "LowPriority7", 10000 );
	std::thread lowprio8(lowpJob, "LowPriority8", 10000 );
	std::thread prio1(pjob, "Prio1", 10000);

	prio1.join();
	lowprio1.join();
	lowprio2.join();
	lowprio3.join();
	lowprio4.join();
	lowprio5.join();
	lowprio6.join();
	lowprio7.join();
	lowprio8.join();

	std::cout << "LOW PRIORITY TEST RESULT:" << std::endl;
	for( auto& elem : durtionTable)
		std::cout << elem.first << " completed by :" << elem.second<< " microsec" << std::endl;

	return 0;
}