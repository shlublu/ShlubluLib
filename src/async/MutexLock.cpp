#include <shlublu/async/MutexLock.h>

#include <shlublu/text/String.h>
#include <shlublu/util/Exceptions.h>


#ifdef _WIN32
#pragma warning (disable: 26110)
#endif


MutexLock::MutexLock(bool take)
	: mLockLevel(0)
{
	if (take)
	{
		queueLock();
	}
}


MutexLock::~MutexLock()
{
	while (mLockLevel > 0)
	{
		unlock();
	}
}


void MutexLock::queueLock() 
{ 
	mMutex.lock();

	mLockLevel++;
	mOwnerId = std::this_thread::get_id();
} 


void MutexLock::unlock() 
{ 
	if (std::this_thread::get_id() == mOwnerId)
	{
		if (mLockLevel > 0)
		{
			mLockLevel--;
			mMutex.unlock();
		}
		else
		{
			throw ShlubluException("MutexLock::unlock(): trying to unlock while lock level is " + String::xtos(mLockLevel));
		}
	}
	else
	{
		throw ShlubluException("MutexLock::unlock(): trying to unlock while not having ownership.");
	}
}

