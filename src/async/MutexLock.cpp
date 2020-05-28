#include <shlublu/async/MutexLock.h>

#include <shlublu/text/String.h>


#ifdef _WIN32
#pragma warning (disable: 26110)
#endif

namespace shlublu
{

MutexLock::MutexLock(bool take)
	: mOwnerId(),
	  mLockLevel(0)
{
	if (take)
	{
		lock();
	}
}


MutexLock::~MutexLock() noexcept
{
	lock();

	while (lockLevel() > 0)
	{
		unlock();
	}
}


void MutexLock::lock() 
{ 
	mMutex.lock();
	mLockLevel++;

	mOwnerId = std::this_thread::get_id();
} 


void MutexLock::unlock() 
{ 
	if (currentThreadIsOwner())
	{
		if (lockLevel() > 0)
		{
			mMutex.unlock();
			mLockLevel--;

			if (!lockLevel())
			{
				mOwnerId = std::thread::id();
			}
		}
		else
		{
			throw LockingError("MutexLock::unlock(): trying to unlock while the lock level is zero.");
		}
	}
	else
	{
		throw LockingError("MutexLock::unlock(): trying to unlock while not having ownership.");
	}
}


unsigned MutexLock::lockLevel() const
{
	return mLockLevel;
}


bool MutexLock::currentThreadIsOwner() const
{
	return std::this_thread::get_id() == mOwnerId;
}

}
