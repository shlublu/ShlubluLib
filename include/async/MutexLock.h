#pragma once

/** @file
	Recursive mutex helper based on <a href="https://www.cplusplus.com/reference/mutex/recursive_mutex/">std::recursive_mutex</a>.

	See MutexLock class documentation for details.
*/

#include <mutex>
#include <thread>


/**
	Recursive mutex. 
	Underlying implementation is based on <a href="https://www.cplusplus.com/reference/mutex/recursive_mutex/">std::recursive_mutex</a>

	<b>Typical examples of use</b>
	@code
	MutexLock lock; // shared by MyThreadedTransactionsManager instances

	// ...

	void MyThreadedTransactionsManager::transactionalEnsembleThatDoesNotSupportConcurrency(int someNumericParameter)
	{
		lock.queueLock();

		// ... operations that do not support concurrency ...

		for (auto i = 0; i < someNumericParameter; ++i)
		{
			singleTransaction(i);
		}

		lock.unlock();
	}

	void MyThreadedTransactionsManager::singleTransaction(int someNumericParameter)
	{
		lock.queueLock();

		// ... operations that do not support concurrency ...

		lock.unlock();
	}
	@endcode
 */
class MutexLock
{
public:
	/**
		Copy constructor is deleted.
	*/
	MutexLock(MutexLock const&) = delete;

	/**
		Move constructor is deleted.
	*/
	MutexLock(MutexLock&&) = delete;

	/**
		Constructor.
		@param locked Initial state of the mutex.
	*/
	MutexLock(bool locked = false);
	
	/**
		Destructor.
		If the mutex is in locked state, unlocks as many time as it has been previously locked.
		@see unlock()
	*/
	virtual ~MutexLock();

public:
	/**
		Blocking lock.
		Queues until the mutex is available for locking. A lock is available if:
		<ul>
		<li>it is in released state
		<li>it has been previously locked by the thread that is currently attempting to lock
		</ul>
	*/
	void queueLock();	

	/**
		Unlocks the mutex.
		Throws a ShlubluException if:
		<ul>
		<li>the mutex is not locked
		<li>the mutex is locked by another thread
		</ul>
	*/
	void unlock();

private:
	std::recursive_mutex mMutex;
	std::thread::id mOwnerId;
	int mLockLevel;
};


