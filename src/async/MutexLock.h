#pragma once

#include <mutex>
#include <thread>


class MutexLock
{
public:
	MutexLock() = delete;
	MutexLock(MutexLock const&) = delete;
	MutexLock(MutexLock&&) = delete;
	MutexLock(bool locked = false);
	virtual ~MutexLock();

public:
	void queueLock();	
	void unlock();	

private:
	std::recursive_mutex mMutex;
	std::thread::id mOwnerId;
	int mLockLevel;
};


