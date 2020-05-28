#define NOMINMAX

#include "CppUnitTest.h"

#include <shlublu/async/MutexLock.h>
#include <shlublu/util/Debug.h>
SHLUBLU_OPTIMIZE_OFF();


using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace shlublu;

namespace async_MutexLock
{
	TEST_CLASS(MutexLockTest)
	{
		TEST_METHOD(MutexLockIsContructedUnlockedByDefault)
		{
			MutexLock lock;
			bool hasJoined(false);

			Assert::AreEqual(0U, lock.lockLevel());

			std::thread t
			(
				[&lock, &hasJoined]() 
				{
					lock.lock();
					Assert::IsFalse(hasJoined);
					lock.unlock();
				}
			);

			using namespace std::chrono_literals;
			std::this_thread::sleep_for(1s);

			t.join();
			hasJoined = true;

			Assert::AreEqual(0U, lock.lockLevel());
		}


		TEST_METHOD(MutexLockCanBeContructedLocked)
		{
			using namespace std::chrono_literals;

			MutexLock lock(true);
			bool hasUnlocked(false);

			Assert::AreEqual(1U, lock.lockLevel());

			std::thread t
			(
				[&lock, &hasUnlocked]()
				{
					lock.lock();
					std::this_thread::sleep_for(1s);
					Assert::IsTrue(hasUnlocked);
					lock.unlock();
				}
			);

			std::this_thread::sleep_for(1s);

			lock.unlock();
			hasUnlocked = true;

			t.join();

			Assert::AreEqual(0U, lock.lockLevel());
		}


		TEST_METHOD(MutexLockLocksProperly)
		{
			MutexLock lock(false);

			for (unsigned i = 0; i < 2; ++i)
			{
				Assert::AreEqual(i, lock.lockLevel());
				lock.lock();
				Assert::AreEqual(i + 1, lock.lockLevel());
			}

			std::thread t
			(
				[&lock]()
				{
					Assert::AreEqual(2U, lock.lockLevel());
					
					lock.lock();
					Assert::AreEqual(1U, lock.lockLevel());
					lock.unlock();
					
					Assert::AreEqual(0U, lock.lockLevel());
				}
			);

			using namespace std::chrono_literals;
			std::this_thread::sleep_for(1s);

			for (unsigned i = 0; i < 2; ++i)
			{
				Assert::AreEqual(2 - i, lock.lockLevel());
				lock.unlock();
				Assert::AreEqual(2 - (i + 1), lock.lockLevel());
			}
			
			t.join();

			Assert::AreEqual(0U, lock.lockLevel());
		}


		TEST_METHOD(MutexLockLevelCountIsCorrect)
		{
			MutexLock lock;
			
			for (unsigned i = 0; i < 10; ++i)
			{
				Assert::AreEqual(i, lock.lockLevel());
				lock.lock();
				Assert::AreEqual(i + 1, lock.lockLevel());
			}

			for (unsigned i = 0; i < 10; ++i)
			{
				Assert::AreEqual(10 - i, lock.lockLevel());
				lock.unlock();
				Assert::AreEqual(10 - (i + 1), lock.lockLevel());
			}
		}


		TEST_METHOD(MutexLockPassesOwnershipProperly)
		{
			MutexLock lock(false);

			Assert::IsFalse(lock.currentThreadIsOwner());

			lock.lock();

			Assert::IsTrue(lock.currentThreadIsOwner());

			std::thread t
			(
				[&lock]()
				{
					Assert::IsFalse(lock.currentThreadIsOwner());

					lock.lock();
					Assert::IsTrue(lock.currentThreadIsOwner());
					lock.unlock();

					Assert::IsFalse(lock.currentThreadIsOwner());
				}
			);

			using namespace std::chrono_literals;
			std::this_thread::sleep_for(1s);

			lock.unlock();
			Assert::IsFalse(lock.currentThreadIsOwner());

			t.join();
		}


		TEST_METHOD(MutexLockOwnershipFollowsLevelProperly)
		{
			MutexLock lock;

			Assert::IsFalse(lock.currentThreadIsOwner());

			for (unsigned i = 0; i < 10; ++i)
			{
				lock.lock();
				Assert::IsTrue(lock.currentThreadIsOwner());
			}

			for (unsigned i = 0; i < 10; ++i)
			{
				Assert::IsTrue(lock.currentThreadIsOwner());
				lock.unlock();
			}

			Assert::IsFalse(lock.currentThreadIsOwner());
		}


		TEST_METHOD(MutexLockThrowsIfUnlockerIsNotOwner)
		{
			MutexLock lock(true);

			std::thread t
			(
				[&lock]()
				{
					Assert::IsFalse(lock.currentThreadIsOwner());
					Assert::ExpectException<ShlubluException>([&lock]() { lock.unlock(); });
				}
			);

			t.join();
		}
	};


	TEST_CLASS(MutexLockGuardTest)
	{
		TEST_METHOD(MutexLockGuardControlsTheMutexProperly)
		{
			MutexLock lock;

			Assert::AreEqual(0U, lock.lockLevel());
			Assert::IsFalse(lock.currentThreadIsOwner());

			{
				MutexLock::Guard guard(lock);
				Assert::AreEqual(1U, lock.lockLevel());
				Assert::IsTrue(lock.currentThreadIsOwner());
			}

			Assert::AreEqual(0U, lock.lockLevel());
			Assert::IsFalse(lock.currentThreadIsOwner());
		}
	};
}

