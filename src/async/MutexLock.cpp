#include <shlublu/async/MutexLock.h>

#include <shlublu/text/String.h>
#include <shlublu/util/Exceptions.h>


#ifdef _WIN32
#pragma warning (disable: 26110)
#endif

namespace shlublu
{

MutexLock::MutexLock(bool take)
	: mLockLevel(0),
	  mOwnerId()
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
			throw ShlubluException("MutexLock::unlock(): trying to unlock while lock level is " + String::xtos(mLockLevel));
		}
	}
	else
	{
		throw ShlubluException("MutexLock::unlock(): trying to unlock while not having ownership.");
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
