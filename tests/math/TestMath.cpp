#define NOMINMAX

#include "CppUnitTest.h"

#include <shlublu/math/Math.h>
#include <shlublu/util/Debug.h>
SHLUBLU_OPTIMIZE_OFF();


using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace shlublu;

namespace math_Math
{
	TEST_CLASS(factorialTest)
	{
	public:
		TEST_METHOD(factorialReturnsProperResult)
		{
			// size_t
			size_t lastSizet(Math::factorial(0));

			Assert::AreEqual(size_t(1), lastSizet);

			for (size_t n = 1; n < 100; ++n)
			{
				const size_t result(Math::factorial(n));
				Assert::AreEqual(n * lastSizet, result);

				lastSizet = result;
			}

			// double
			double lastDouble(Math::factorial(0));

			Assert::AreEqual(double(1), lastDouble);

			for (double n = 1; n < 100; ++n)
			{
				const double result(Math::factorial(n));
				Assert::AreEqual(n * lastDouble, result);

				lastDouble = result;
			}
		}

		TEST_METHOD(factorialThrowsIfNegativeN)
		{
			Assert::ExpectException<std::domain_error>([]() { return Math::factorial(-1); });
			Assert::ExpectException<std::domain_error>([]() { return Math::factorial(-std::numeric_limits<double>::epsilon()); });
			Assert::ExpectException<std::domain_error>([]() { return Math::factorial(-std::numeric_limits<float>::epsilon()); });
		}


		TEST_METHOD(factorialThrowsIfNotRoundN)
		{
			Assert::ExpectException<std::domain_error>([]() { return Math::factorial(0.1); });
			Assert::ExpectException<std::domain_error>([]() { return Math::factorial(0.1f); });
		}
	};


	TEST_CLASS(clampTest)
	{
	public:
		TEST_METHOD(clampCutsWhatItShouldButLeavesTheRestUnchanged)
		{
			Assert::AreEqual(2, Math::clamp(-6, 2, 5));
			Assert::AreEqual(2, Math::clamp(2, 2, 5));
			Assert::AreEqual(3, Math::clamp(3, 2, 5));
			Assert::AreEqual(5, Math::clamp(5, 2, 5));
			Assert::AreEqual(5, Math::clamp(8, 2, 5));
			Assert::AreEqual(-5, Math::clamp(8, -5, -5));

			Assert::AreEqual(2.0, Math::clamp(-6.0, 2.0, 5.0));
			Assert::AreEqual(2.0, Math::clamp(2.0, 2.0, 5.0));
			Assert::AreEqual(3.0, Math::clamp(3.0, 2.0, 5.0));
			Assert::AreEqual(5.0, Math::clamp(5.0, 2.0, 5.0));
			Assert::AreEqual(5.0, Math::clamp(8.0, 2.0, 5.0));
			Assert::AreEqual(-5.0, Math::clamp(8.0, -5.0, -5.0));

			Assert::AreEqual(2.0f, Math::clamp(-6.0f, 2.0f, 5.0f));
			Assert::AreEqual(2.0f, Math::clamp(2.0f, 2.0f, 5.0f));
			Assert::AreEqual(3.0f, Math::clamp(3.0f, 2.0f, 5.0f));
			Assert::AreEqual(5.0f, Math::clamp(5.0f, 2.0f, 5.0f));
			Assert::AreEqual(5.0f, Math::clamp(8.0f, 2.0f, 5.0f));
			Assert::AreEqual(-5.0f, Math::clamp(8.0f, -5.0f, -5.0f));
		}


		TEST_METHOD(clampThrowsWhenMinIsGreaterThanMax)
		{
			Assert::ExpectException<std::invalid_argument>([]() { return Math::clamp(10, 100, 1); });
		}
	};


	TEST_CLASS(roundXTest)
	{
	public:
		TEST_METHOD(roundXGivesProperResult)
		{
			// double
			Assert::AreEqual(4.0, Math::roundX(4.2356, 0));
			Assert::AreEqual(5.0, Math::roundX(4.5356, 0));

			Assert::AreEqual(4.2, Math::roundX(4.2356, 1));
			Assert::AreEqual(4.3, Math::roundX(4.2556, 1));

			Assert::AreEqual(4.23, Math::roundX(4.2346, 2));
			Assert::AreEqual(4.24, Math::roundX(4.2356, 2));

			Assert::AreEqual(4.234, Math::roundX(4.2343, 3));
			Assert::AreEqual(4.235, Math::roundX(4.2345, 3));

			// float
			Assert::AreEqual(4.0f, Math::roundX(4.2356f, 0));
			Assert::AreEqual(5.0f, Math::roundX(4.5356f, 0));

			Assert::AreEqual(4.2f, Math::roundX(4.2356f, 1));
			Assert::AreEqual(4.3f, Math::roundX(4.2556f, 1));

			Assert::AreEqual(4.23f, Math::roundX(4.2346f, 2));
			Assert::AreEqual(4.24f, Math::roundX(4.2356f, 2));

			Assert::AreEqual(4.234f, Math::roundX(4.2343f, 3));
			Assert::AreEqual(4.235f, Math::roundX(4.2345f, 3));
		}
		
		TEST_METHOD(round2GivesRoundX)
		{
			// double
			Assert::AreEqual(Math::round2(4.2346), Math::roundX(4.2346, 2));
			Assert::AreEqual(Math::round2(4.5346), Math::roundX(4.5346, 2));

			// float
			Assert::AreEqual(Math::round2(4.2346f), Math::roundX(4.2346f, 2));
			Assert::AreEqual(Math::round2(4.5346f), Math::roundX(4.5346f, 2));
		}
	};


	TEST_CLASS(proportionalIncreaseTest)
	{
	public:
		TEST_METHOD(proportionalIncreaseGivesProperResultWithValuesOfSameSign)
		{
			// double
			Assert::AreEqual(1.0, Math::proportionalIncrease(1.0, 2.0));
			Assert::AreEqual(-0.5, Math::proportionalIncrease(1.0, 0.5));
			Assert::AreEqual(0.0, Math::proportionalIncrease(4.5, 4.5));
			Assert::AreEqual(1.0, Math::proportionalIncrease(-1.0, -2.0));
			Assert::AreEqual(-0.5, Math::proportionalIncrease(-1.0, -0.5));
			Assert::AreEqual(0.0, Math::proportionalIncrease(-4.5, -4.5));

			// float
			Assert::AreEqual(1.0f, Math::proportionalIncrease(1.0f, 2.0f));
			Assert::AreEqual(-0.5f, Math::proportionalIncrease(1.0f, 0.5f));
			Assert::AreEqual(0.0f, Math::proportionalIncrease(4.5f, 4.5f));
			Assert::AreEqual(1.0f, Math::proportionalIncrease(-1.0f, -2.0f));
			Assert::AreEqual(-0.5f, Math::proportionalIncrease(-1.0f, -0.5f));
			Assert::AreEqual(0.0f, Math::proportionalIncrease(-4.5f, -4.5f));
		}


		TEST_METHOD(proportionalIncreaseGivesProperResultWithDistinctIncreaseAndValueTypes)
		{
			// double
			Assert::AreEqual(1.0, Math::proportionalIncrease<int, double>(1, 2));
			Assert::AreEqual(-0.5, Math::proportionalIncrease<int, double>(2, 1));
			Assert::AreEqual(0.0, Math::proportionalIncrease<int, double>(4, 4));
			Assert::AreEqual(1.0, Math::proportionalIncrease<int, double>(-1, -2));
			Assert::AreEqual(-0.5, Math::proportionalIncrease<int, double>(-2, -1));
			Assert::AreEqual(0.0, Math::proportionalIncrease<int, double>(-4, -4));

			// float
			Assert::AreEqual(1.0f, Math::proportionalIncrease<int, float>(1, 2));
			Assert::AreEqual(-0.5f, Math::proportionalIncrease<int, float>(2, 1));
			Assert::AreEqual(0.0f, Math::proportionalIncrease<int, float>(4, 4));
			Assert::AreEqual(1.0f, Math::proportionalIncrease<int, float>(-1, -2));
			Assert::AreEqual(-0.5f, Math::proportionalIncrease<int, float>(-2, -1));
			Assert::AreEqual(0.0f, Math::proportionalIncrease<int, float>(-4, -4));
		}


		TEST_METHOD(proportionalIncreaseGivesProperResultWithFinalValueOfZero)
		{
			// double
			Assert::AreEqual(-1.0, Math::proportionalIncrease(4.0, 0.0));
			Assert::AreEqual(-1.0, Math::proportionalIncrease(-4.0, 0.0));

			// float
			Assert::AreEqual(-1.0f, Math::proportionalIncrease(4.0f, 0.0f));
			Assert::AreEqual(-1.0f, Math::proportionalIncrease(-4.0f, 0.0f));
		}


		TEST_METHOD(proportionalIncreaseGivesProperResultWithValuesOfOppositeSign)
		{
			// double
			Assert::AreEqual(-3.0, Math::proportionalIncrease(-1.0, 2.0));
			Assert::AreEqual(-1.5, Math::proportionalIncrease(1.0, -0.5));

			// float
			Assert::AreEqual(-3.0f, Math::proportionalIncrease(-1.0f, 2.0f));
			Assert::AreEqual(-1.5f, Math::proportionalIncrease(1.0f, -0.5f));
		}

		TEST_METHOD(increaseGivesThrowsIfInitialValueIsZero)
		{
			Assert::ExpectException<std::invalid_argument>([]() { Math::proportionalIncrease(0.0, 1.0); });
		}
	};


	TEST_CLASS(increaseRateTest)
	{
	public:
		TEST_METHOD(increaseRateGivesProperResultWithProperPositiveValues)
		{
			// double
			Assert::AreEqual(0.012251749322662375, Math::increaseRate(0.23, 17));
			Assert::AreEqual(0.071773462536293131, Math::increaseRate(1.0, 10));
			Assert::AreEqual(0.011908881146029060, Math::increaseRate(4.5, 144));

			// float
			Assert::AreEqual(0.0122517347f, Math::increaseRate(0.23f, 17));
			Assert::AreEqual(0.0717734098f, Math::increaseRate(1.0f, 10));
			Assert::AreEqual(0.0119088888f, Math::increaseRate(4.5f, 144));
		}


		TEST_METHOD(increaseRateGivesProperResultWithProperNegativeValues)
		{
			// double
			Assert::AreEqual(-0.97279529489699612, Math::increaseRate(-1.0 + std::numeric_limits<double>::epsilon(), 10));
			Assert::AreEqual(-0.0021831048363700756, Math::increaseRate(-0.27, 144));

			// float
			Assert::AreEqual(-0.796936870f, Math::increaseRate(-1.0f + std::numeric_limits<float>::epsilon(), 10));
			Assert::AreEqual(-0.00218307972f, Math::increaseRate(-0.27f, 144));
		}


		TEST_METHOD(increaseRateThrowsWithZeroUnits)
		{
			Assert::ExpectException<std::invalid_argument>([]() { return Math::increaseRate(1.0, 0); });
		}
		

		TEST_METHOD(increaseRateThrowsWithDecreaseExceedingInitialValue)
		{
			// double
			Assert::ExpectException<std::domain_error>([]() { return Math::increaseRate(-1.0, 10); });
			Assert::ExpectException<std::domain_error>([]() { return Math::increaseRate(-2.0, 10); });

			// float
			Assert::ExpectException<std::domain_error>([]() { return Math::increaseRate(-1.0f, 10); });
			Assert::ExpectException<std::domain_error>([]() { return Math::increaseRate(-2.0f, 10); });
		}


		TEST_METHOD(increaseRateConvenienceBehavesCorrectly)
		{
			// double
			Assert::AreEqual(Math::increaseRate(Math::proportionalIncrease(1.0, 5.0), 10), Math::increaseRate(1.0, 5.0, 10));
			Assert::AreEqual(Math::increaseRate(Math::proportionalIncrease(5.0, 1.0), 10), Math::increaseRate(5.0, 1.0, 10));
			Assert::AreEqual(Math::increaseRate(Math::proportionalIncrease<unsigned, double>(1, 5), 10), Math::increaseRate<unsigned, double>(1, 5, 10));
			Assert::AreEqual(Math::increaseRate(Math::proportionalIncrease<unsigned, double>(5, 1), 10), Math::increaseRate<unsigned, double>(5, 1, 10));

			// float
			Assert::AreEqual(Math::increaseRate(Math::proportionalIncrease(1.0f, 5.0f), 10), Math::increaseRate(1.0f, 5.0f, 10));
			Assert::AreEqual(Math::increaseRate(Math::proportionalIncrease(5.0f, 1.0f), 10), Math::increaseRate(5.0f, 1.0f, 10));
			Assert::AreEqual(Math::increaseRate(Math::proportionalIncrease<unsigned, double>(1, 5), 10), Math::increaseRate<unsigned, double>(1, 5, 10));
			Assert::AreEqual(Math::increaseRate(Math::proportionalIncrease<unsigned, double>(5, 1), 10), Math::increaseRate<unsigned, double>(5, 1, 10));
		}
	};


	TEST_CLASS(sameSignTest)
	{
	public:
		TEST_METHOD(sameSignReturnsProperResult)
		{
			// int
			Assert::IsTrue(Math::sameSign(0, 5));
			Assert::IsTrue(Math::sameSign(-5, -4));
			Assert::IsTrue(Math::sameSign(45, 47));

			Assert::IsFalse(Math::sameSign(0, -5));
			Assert::IsFalse(Math::sameSign(5, -4));
			Assert::IsFalse(Math::sameSign(-45, 47));

			// double
			Assert::IsTrue(Math::sameSign(-0.0, 0.0));

			Assert::IsTrue(Math::sameSign(0.0, 5.0));
			Assert::IsTrue(Math::sameSign(-5.0, -4.0));
			Assert::IsTrue(Math::sameSign(45.0, 47.0));

			Assert::IsFalse(Math::sameSign(0.0, -5.0));
			Assert::IsFalse(Math::sameSign(5.0, -4.0));
			Assert::IsFalse(Math::sameSign(-45.0, 47.0));
		}
	};
}
