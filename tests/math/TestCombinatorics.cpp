#define NOMINMAX

#include "CppUnitTest.h"

#include <shlublu/math/Combinatorics.h>
#include <shlublu/util/Debug.h>
SHLUBLU_OPTIMIZE_OFF();


using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace shlublu;

namespace math_Combinatorics
{
	TEST_CLASS(CombinationsTest)
	{
	public:
		TEST_METHOD(CombinationsConstructionIsCorrect)
		{
			constexpr size_t n(5);
			constexpr size_t k(3);

			const Combination combi(n, k);

			Assert::AreEqual(n, combi.n());
			Assert::AreEqual(k, combi.k());
		}


		TEST_METHOD(CombinationsConstructionThrowsIfKGreaterThanN)
		{
			Assert::ExpectException<std::invalid_argument>([]() { return Combination(5, 6).number(); });
		}


		TEST_METHOD(CombinationsHasTheProperNumber)
		{
			Assert::AreEqual(size_t(924), Combination(12, 6).number());
			Assert::AreEqual(size_t(5), Combination(5, 4).number());
			
			Assert::AreEqual(size_t(1), Combination(8, 8).number());

			const Combination combi(14, 1);
			Assert::AreEqual(size_t(combi.n()), combi.number());
		}


		TEST_METHOD(CombinationsHasTheProperStaticNumber)
		{
			Assert::AreEqual(size_t(924), Combination::number(12, 6));
			Assert::AreEqual(size_t(5), Combination::number(5, 4));

			Assert::AreEqual(size_t(1), Combination::number(8, 8));
			Assert::AreEqual(size_t(14), Combination::number(14, 1));
		}


		TEST_METHOD(CombinationsRollsAsExepected)
		{
			Combination combi(12, 6);

			size_t ctr(0);
			while (combi.next())
			{
				ctr++;
			}

			Assert::AreEqual(combi.number(), ctr);
		}

		TEST_METHOD(CombinationsIsComplete)
		{
			Combination combi(5, 4);

			Assert::IsTrue(combi.next());
			Assert::IsTrue(std::vector<size_t>{ {0, 1, 2, 3} } == combi.kUplet());

			Assert::IsTrue(combi.next());
			Assert::IsTrue(std::vector<size_t>{ {0, 1, 2, 4} } == combi.kUplet());

			Assert::IsTrue(combi.next());
			Assert::IsTrue(std::vector<size_t>{ {0, 1, 3, 4} } == combi.kUplet());

			Assert::IsTrue(combi.next());
			Assert::IsTrue(std::vector<size_t>{ {0, 2, 3, 4} } == combi.kUplet());

			Assert::IsTrue(combi.next());
			Assert::IsTrue(std::vector<size_t>{ {1, 2, 3, 4} } == combi.kUplet());

			Assert::IsFalse(combi.next());
		}
	};


	TEST_CLASS(ArrangementsTest)
	{
	public:
		TEST_METHOD(ArrangementsConstructionIsCorrect)
		{
			constexpr size_t n(5);
			constexpr size_t k(3);

			const Arrangement argt(n, k);

			Assert::AreEqual(n, argt.n());
			Assert::AreEqual(k, argt.k());
		}


		TEST_METHOD(ArrangementsConstructionThrowsIfKGreaterThanN)
		{
			Assert::ExpectException<std::logic_error>([]() { return Arrangement(5, 6).number(); });
		}


		TEST_METHOD(ArrangementsHasTheProperNumber)
		{
			Assert::AreEqual(size_t(665280), Arrangement(12, 6).number());
			Assert::AreEqual(size_t(6), Arrangement(3, 2).number());

			Assert::AreEqual(size_t(40320), Arrangement(8, 8).number());

			const Arrangement combi(14, 1);
			Assert::AreEqual(size_t(combi.n()), combi.number());
		}


		TEST_METHOD(ArrangementsHasTheProperStaticNumber)
		{
			Assert::AreEqual(size_t(665280), Arrangement::number(12, 6));
			Assert::AreEqual(size_t(6), Arrangement::number(3, 2));

			Assert::AreEqual(size_t(40320), Arrangement::number(8, 8));
			Assert::AreEqual(size_t(14), Arrangement::number(14, 1));
		}

		
		TEST_METHOD(ArrangementsRollsAsExepected)
		{
			Arrangement argts(12, 6);

			size_t ctr(0);
			while (argts.next())
			{
				ctr++;
			}

			Assert::AreEqual(argts.number(), ctr);
		}


		TEST_METHOD(ArrangementsIsComplete)
		{
			Arrangement argt(3, 2);

			Assert::IsTrue(argt.next());
			Assert::IsTrue(std::vector<size_t>{ {0, 1} } == argt.kUplet());

			Assert::IsTrue(argt.next());
			Assert::IsTrue(std::vector<size_t>{ {1, 0} } == argt.kUplet());

			Assert::IsTrue(argt.next());
			Assert::IsTrue(std::vector<size_t>{ {0, 2} } == argt.kUplet());

			Assert::IsTrue(argt.next());
			Assert::IsTrue(std::vector<size_t>{ {2, 0} } == argt.kUplet());

			Assert::IsTrue(argt.next());
			Assert::IsTrue(std::vector<size_t>{ {1, 2} } == argt.kUplet());

			Assert::IsTrue(argt.next());
			Assert::IsTrue(std::vector<size_t>{ {2, 1} } == argt.kUplet());

			Assert::IsFalse(argt.next());
		}
	};
}
