#pragma once

/** @file
	Recursive mutex helper based on <a href="https://www.cplusplus.com/reference/mutex/recursive_mutex/">std::recursive_mutex</a>.

	See MutexLock class documentation for details.
*/

#include <mutex>
#include <thread>

namespace shlublu
{

/**
	Recursive mutex. 
	Underlying implementation is based on <a href="https://www.cplusplus.com/reference/mutex/recursive_mutex/">std::recursive_mutex</a>

	<b>Typical examples of use</b>
	@code
	MutexLock locker; // shared by MyThreadedTransactionsManager instances

	// ...

	void MyThreadedTransactionsManager::transactionalEnsembleThatDoesNotSupportConcurrency(int someNumericParameter)
	{
		// ... operations that support concurrency ...

		locker.lock();

		// ... operations that do not support concurrency ...

		for (auto i = 0; i < someNumericParameter; ++i)
		{
			singleTransaction(i);
		}

		locker.unlock();

		// ... operations that support concurrency ...
	}

	void MyThreadedTransactionsManager::singleTransaction(int someNumericParameter)
	{
		// ... operations that support concurrency ...

		locker.lock();

		// ... operations that do not support concurrency ...

		locker.unlock();

		// ... operations that support concurrency ...
	}
	@endcode
 */
class MutexLock
{
public:
	/**
		<a href="https://en.cppreference.com/w/cpp/language/raii">RAII</a> convenience wrapper.

		This wrapper guarantees that the mutex will be unlock at the moment its destructor will be called. Used in a function or method, this
		makes `MutexLock::unlock()` to be called at `return` or `throw` time with no further manual handling.

		Based on the standard `std::lock_guard` implementation.
		
		@see <a href="https://www.cplusplus.com/reference/mutex/lock_guard/">std::lock_guard</a>

		<b>Example</b>
		@code
		MutexLock locker; // let's assume this is a memeber of MyThreadedTransactionsManager

		// ...

		int MyThreadedTransactionsManager::methodThatDoesNotSupportConcurrency()
		{
			MutexLock::Guard(locker);

			int x(someCall());

			// ... operations that do not support concurrency ...

			return x;
		}
		@endcode
	*/
	using Guard = std::lock_guard<MutexLock>;

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
		@exception ShlubluException if the mutex is locked by another thread
		@see unlock()
	*/
	virtual ~MutexLock();

public:
	/**
		Locks the mutex once available.
		Queues until the mutex is available for locking. A lock is available if:
		<ul>
		<li>it is in released state
		<li>it has been previously locked by the same thread as the calling one
		</ul>
	*/
	void lock();	

	/**
		Unlocks the mutex.
		@exception ShlubluException if the mutex is not locked or if the mutex is locked by another thread
	*/
	void unlock();

private:
	/// @cond INTERNAL

	std::recursive_mutex mMutex;
	std::thread::id mOwnerId;
	int mLockLevel;

	/// @endcond
};

}
