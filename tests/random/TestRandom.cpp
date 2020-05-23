#define NOMINMAX

#include "CppUnitTest.h"

#include <shlublu/random/Random.h>
#include <shlublu/util/Debug.h>
SHLUBLU_OPTIMIZE_OFF();


using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace shlublu;

namespace random_Random
{
	TEST_CLASS(initTest)
	{
	public:
		TEST_METHOD(engineInitializesProperly)
		{
			Assert::IsNotNull(Random::__sRandomEngine);
		}
	};


	TEST_CLASS(randomTest)
	{
	public:
		TEST_METHOD(randomRespectsBoundaries)
		{
			constexpr size_t nbAttempts(1000000);

			// Real
			constexpr double minD(-10.0);
			constexpr double maxD(10.0);

			for (size_t ctr = 0; ctr < nbAttempts; ++ctr)
			{
				const auto r(Random::random(minD, maxD));
				Assert::IsTrue(r >= minD && r < maxD);
			}

			// Integer
			constexpr int minI(-100);
			constexpr int maxI(100);

			for (size_t ctr = 0; ctr < nbAttempts; ++ctr)
			{
				const auto r(Random::random(minI, maxI));
				Assert::IsTrue(r >= minI && r <= maxI);
			}
		}


		TEST_METHOD(randomIsProperlyDistributed)
		{
			constexpr size_t nbAttempts(1000000);

			const size_t expectedMin(static_cast<size_t>(std::round((nbAttempts / 10.0) * 0.9)));
			const size_t expectedMax(static_cast<size_t>(std::round((nbAttempts / 10.0) * 1.1)));

			// real
			constexpr double minD(0.0);
			constexpr double maxD(10.0);

			size_t resultsD[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

			for (size_t ctr = 0; ctr < nbAttempts; ++ctr)
			{
				++resultsD[static_cast<size_t>(Random::random(minD, maxD))];
			}

			for (size_t slot = 0; slot < 10; ++slot)
			{
				const auto resultsForSlot(resultsD[slot]);

				Assert::IsTrue(resultsForSlot >= expectedMin && resultsForSlot <= expectedMax);
			}

			// integer
			constexpr int minI(0);
			constexpr int maxI(10);

			size_t resultsI[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

			for (size_t ctr = 0; ctr < nbAttempts; ++ctr)
			{
				++resultsI[Random::random(minI, maxI)];
			}

			for (size_t slot = 0; slot < 10; ++slot)
			{
				const auto resultsForSlot(resultsI[slot]);

				Assert::IsTrue(resultsForSlot >= expectedMin && resultsForSlot <= expectedMax);
			}
		}


		TEST_METHOD(randomThrowsIfMinIsGreaterThanMax)
		{
			Assert::ExpectException<ShlubluException>([]() { Random::random(1.0, 0.9); });
			Assert::ExpectException<ShlubluException>([]() { Random::random(1.0f, 0.9f); });
			Assert::ExpectException<ShlubluException>([]() { Random::random(10, 9); });
			Assert::ExpectException<ShlubluException>([]() { Random::random(10, -11); });
			Assert::ExpectException<ShlubluException>([]() { Random::random(10U, 9U); });
		}
	};


	TEST_CLASS(randomRoundedTest)
	{
	public:
		TEST_METHOD(randomRoundedRespectsBoundaries)
		{
			constexpr size_t nbAttempts(1000000);

			// Real
			constexpr double minD(-10.0);
			constexpr double maxD(10.0);
			constexpr double stepD(0.1);

			for (size_t ctr = 0; ctr < nbAttempts; ++ctr)
			{
				const auto r(Random::random(minD, maxD, stepD));
				Assert::IsTrue(r >= minD && r <= maxD);
			}

			// Integer
			constexpr int minI(-100);
			constexpr int maxI(100);
			constexpr int stepI(2);

			for (size_t ctr = 0; ctr < nbAttempts; ++ctr)
			{
				const auto r(Random::random(minI, maxI, stepI));
				Assert::IsTrue(r >= minI && r <= maxI);
			}
		}


		TEST_METHOD(randomRoundedIsProperlyDistributed)
		{
			constexpr size_t nbAttempts(1000000);

			const size_t expectedMin(static_cast<size_t>(std::round((nbAttempts / 10.0) * 0.9)));
			const size_t expectedMax(static_cast<size_t>(std::round((nbAttempts / 10.0) * 1.1)));

			// real
			const size_t expectedTwentiethMin(static_cast<size_t>(std::round((nbAttempts / 10.0 / 20.0) * 0.9)));
			const size_t expectedTwentiethMax(static_cast<size_t>(std::round((nbAttempts / 10.0 / 20.0) * 1.1)));

			constexpr double minD(0.0);
			constexpr double maxD(10.0);
			constexpr double stepD(0.1);

			size_t resultsD[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

			for (size_t ctr = 0; ctr < nbAttempts; ++ctr)
			{
				++resultsD[static_cast<size_t>(Random::random(minD, maxD, stepD))];
			}

			for (size_t slot = 0; slot < 11; ++slot)
			{
				const auto resultsForSlot(resultsD[slot]);

				if (slot < 10)
				{
					Assert::IsTrue(resultsForSlot >= expectedMin && resultsForSlot <= expectedMax);
				}
				else
				{
					Assert::IsTrue(resultsForSlot >= expectedTwentiethMin && resultsForSlot <= expectedTwentiethMax);
				}
			}

			// integer
			const size_t expectedHalfMin(static_cast<size_t>(std::round((nbAttempts / 10.0 / 2.0) * 0.9)));
			const size_t expectedHalfMax(static_cast<size_t>(std::round((nbAttempts / 10.0 / 2.0) * 1.1)));

			constexpr int minI(0);
			constexpr int maxI(100);
			constexpr int stepI(10);

			size_t resultsI[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

			for (size_t ctr = 0; ctr < nbAttempts; ++ctr)
			{
				++resultsI[Random::random(minI, maxI, stepI) / 10];
			}

			for (size_t slot = 0; slot < 11; ++slot)
			{
				const auto resultsForSlot(resultsI[slot]);

				if (slot > 0 && slot < 10)
				{
					Assert::IsTrue(resultsForSlot >= expectedMin && resultsForSlot <= expectedMax);
				}
				else
				{
					Assert::IsTrue(resultsForSlot >= expectedHalfMin && resultsForSlot <= expectedHalfMax);
				}
			}
		}


		TEST_METHOD(randomRoundedIsRound)
		{
			constexpr size_t nbAttempts(10000);

			for (size_t i = 0; i < nbAttempts; ++i)
			{
				auto rD(Random::random(-100.75432, 100.18645, 0.01));
				Assert::AreEqual(std::round(100.0 * rD) / 100.0, rD);

				auto rIEven(Random::random(-100, 100, 2));
				Assert::IsFalse(rIEven & 1);
			}
		}


		TEST_METHOD(randomRoundedThrowsIfMinIsGreaterThanMax)
		{
			Assert::ExpectException<ShlubluException>([]() { Random::random(1.0, 0.9, 0.1); });
			Assert::ExpectException<ShlubluException>([]() { Random::random(1.0f, 0.9f, 0.1f); });
			Assert::ExpectException<ShlubluException>([]() { Random::random(10, 9, 1); });
			Assert::ExpectException<ShlubluException>([]() { Random::random(10, -11, 1); });
			Assert::ExpectException<ShlubluException>([]() { Random::random(10U, 9U, 1U); });
		}


		TEST_METHOD(randomRoundedThrowsIfStepIsNegative)
		{
			Assert::ExpectException<ShlubluException>([]() { Random::random(1.0, 2.0, -0.1); });
			Assert::ExpectException<ShlubluException>([]() { Random::random(1.0f, 2.0f, -0.1f); });
			Assert::ExpectException<ShlubluException>([]() { Random::random(10, 11, -1); });
			Assert::ExpectException<ShlubluException>([]() { Random::random(-10, 11, -1); });
		}
	};


	TEST_CLASS(randomUnitTest)
	{
	public:
		TEST_METHOD(randomUnitRespectsBoundaries)
		{
			constexpr size_t nbAttempts(1000000);

			for (size_t ctr = 0; ctr < nbAttempts; ++ctr)
			{
				const auto rD(Random::randomUnit<double>());
				Assert::IsTrue(rD >= 0.0 && rD <= 1.0);

				const auto rI(Random::randomUnit<int>());
				Assert::IsTrue(rI >= 0 && rI <= 1);
			}
		}
	};


	TEST_CLASS(randomRelativeUnitTest)
	{
	public:
		TEST_METHOD(randomRelativeUnitRespectsBoundaries)
		{
			constexpr size_t nbAttempts(1000000);

			for (size_t ctr = 0; ctr < nbAttempts; ++ctr)
			{
				const auto rD(Random::randomRelativeUnit<double>());
				Assert::IsTrue(rD >= -1.0 && rD <= 1.0);

				const auto rI(Random::randomRelativeUnit<int>());
				Assert::IsTrue(rI >= -1 && rI <= 1);
			}
		}
	};


	TEST_CLASS(probabilityTest)
	{
	public:
		TEST_METHOD(probabilityIsProperlyDistributed)
		{
			constexpr size_t nbAttempts(1000000);

			for (auto probability : { 0.0, 1.0, 0.75, 0.25, 0.6667, 0.3333, 0.267489 })
			{
				const size_t expectedSuccessMin(static_cast<size_t>(nbAttempts * probability * 0.9));
				const size_t expectedSuccessMax(static_cast<size_t>(nbAttempts * probability * 1.1));

				const size_t expectedFailuresMin(static_cast<size_t>(nbAttempts * (1.0 - probability) * 0.9));
				const size_t expectedFailuresMax(static_cast<size_t>(nbAttempts * (1.0 - probability) * 1.1));

				size_t successes(0);
				size_t failures(0);

				for (size_t ctr = 0; ctr < nbAttempts; ++ctr)
				{
					if (Random::probability(probability))
					{
						successes++;
					}
					else
					{
						failures++;
					}
				}

				Assert::IsTrue(successes >= expectedSuccessMin && successes <= expectedSuccessMax);
				Assert::IsTrue(failures >= expectedFailuresMin && failures <= expectedFailuresMax);
			}
		}


		TEST_METHOD(probabilityThrowsIfProbabilityIsNegative)
		{
			Assert::ExpectException<ShlubluException>([]() { Random::probability(-std::numeric_limits<double>::min()); });
			Assert::ExpectException<ShlubluException>([]() { Random::probability(-std::numeric_limits<float>::min()); });
		}


		TEST_METHOD(probabilityThrowsIfProbabilityExceedsOne)
		{
			Assert::ExpectException<ShlubluException>([]() { Random::probability(1.0 + std::numeric_limits<double>::epsilon()); });
			Assert::ExpectException<ShlubluException>([]() { Random::probability(1.0f + std::numeric_limits<float>::epsilon()); });
		}
	};


	TEST_CLASS(likelihoodTest)
	{
	public:
		TEST_METHOD(likelihoodIsProperlyDistributed)
		{
			constexpr size_t nbAttempts(1000000);

			constexpr unsigned total(51);

			for (unsigned chances : { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 })
			{
				const size_t expectedSuccessMin(static_cast<size_t>(nbAttempts * (static_cast<double>(chances) / static_cast<double>(total)) * 0.9));
				const size_t expectedSuccessMax(static_cast<size_t>(nbAttempts * (static_cast<double>(chances) / static_cast<double>(total)) * 1.1));

				const size_t expectedFailuresMin(static_cast<size_t>(nbAttempts * (1.0 - (static_cast<double>(chances) / static_cast<double>(total))) * 0.9));
				const size_t expectedFailuresMax(static_cast<size_t>(nbAttempts * (1.0 - (static_cast<double>(chances) / static_cast<double>(total))) * 1.1));

				size_t successes(0);
				size_t failures(0);

				for (size_t ctr = 0; ctr < nbAttempts; ++ctr)
				{
					if (Random::likelihood(chances, total))
					{
						successes++;
					}
					else
					{
						failures++;
					}
				}

				Assert::IsTrue(successes >= expectedSuccessMin && successes <= expectedSuccessMax);
				Assert::IsTrue(failures >= expectedFailuresMin && failures <= expectedFailuresMax);
			}
		}


		TEST_METHOD(likelihoodThrowsIfMoreChancesThanTotal)
		{
			Assert::ExpectException<ShlubluException>([]() { Random::likelihood(2U, 1U); });
		}
	};


	TEST_CLASS(tossACoinTest)
	{
	public:
		TEST_METHOD(tossACoinIsProperlyDistributed)
		{
			constexpr size_t nbAttempts(1000000);

			constexpr double probability(0.5);

			const size_t expectedMin(static_cast<size_t>(nbAttempts * probability * 0.9));
			const size_t expectedMax(static_cast<size_t>(nbAttempts * probability * 1.1));

			size_t successes(0);
			size_t failures(0);

			for (size_t ctr = 0; ctr < nbAttempts; ++ctr)
			{
				if (Random::tossACoin())
				{
					successes++;
				}
				else
				{
					failures++;
				}
			}

			Assert::IsTrue(successes >= expectedMin && successes <= expectedMax);
			Assert::IsTrue(failures >= expectedMin && failures <= expectedMax);
		}
	};
}
