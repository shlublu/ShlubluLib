#define NOMINMAX

#include "CppUnitTest.h"

#include <shlublu/hash/CRC.h>
#include <shlublu/util/Debug.h>
SHLUBLU_OPTIMIZE_OFF();


using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace shlublu;

namespace hash_CRC
{
	TEST_CLASS(CRC64Test)
	{
		TEST_METHOD(CRC64IsProperlyConstructedEmpty)
		{
			const CRC64 crc;

			Assert::AreEqual(crc64_t(0), crc.get());
			Assert::AreEqual(8ULL, sizeof(crc.get()));
		}


		TEST_METHOD(CRC64IsProperlyConstructedWithParam)
		{
			Assert::AreEqual(crc64_t(16729334732228315130), CRC64(22).get());

			Assert::AreEqual(crc64_t(8121845331507789120), CRC64(-112).get());

			Assert::AreEqual(crc64_t(17075131938375476053), CRC64(-112LL).get());

			Assert::AreEqual(crc64_t(12900091222520457477), CRC64(58.12f).get());

			Assert::AreEqual(crc64_t(2139392401637288023), CRC64(58.12).get());

			Assert::AreEqual(crc64_t(3118128885020022634), CRC64("TEST").get());
			Assert::AreEqual(crc64_t(3118128885020022634), CRC64(std::string("TEST")).get());
			Assert::AreEqual(crc64_t(3118128885020022634), CRC64(std::vector<char>{ {'T', 'E', 'S', 'T'} }).get());
		}


		TEST_METHOD(CRC64AccumulatesStringsProperly)
		{
			CRC64 crc;

			crc.accumulate(std::string("TEST"));

			Assert::AreEqual(crc64_t(3118128885020022634), crc.get());
		}


		TEST_METHOD(CRC64AccumulatesCharArraysProperly)
		{
			CRC64 crc;

			crc.accumulate("TEST");

			Assert::AreEqual(crc64_t(3118128885020022634), crc.get());
		}


		TEST_METHOD(CRC64AccumulatesVectorsProperly)
		{
			CRC64 crc;

			crc.accumulate(std::vector<char>{ {'T', 'E', 'S', 'T'} });

			Assert::AreEqual(crc64_t(3118128885020022634), crc.get());
		}


		TEST_METHOD(CRC64AccumulatesArithmeticTypesProperly)
		{
			CRC64 crc;

			crc.accumulate('a');
			crc.accumulate(-1);
			crc.accumulate(25LL);
			crc.accumulate(12.55f);
			crc.accumulate(1e-13);

			Assert::AreEqual(crc64_t(9965139386617658183), crc.get());
		}


		TEST_METHOD(CRC64AccumulatesSlicesProperly)
		{
			CRC64 crc;

			crc.accumulate("LKJTESTMLK", 3, 4);

			Assert::AreEqual(crc64_t(3118128885020022634), crc.get());
		}


		TEST_METHOD(CRC64AccumulatesRandomIbjectsProperly)
		{
			class X
			{
			public:
				X(int a, double b) : mA(a), mB(b) {}

			private:
				int mA;
				double mB;
			};

			const X x(42, 42.0);
			CRC64 crc;

			crc.accumulate(reinterpret_cast<char const*>(&x), 0, sizeof(int));

			Assert::AreEqual(crc.get(), CRC64(42).get());
		}


		TEST_METHOD(CRC64AccumulatesContainersProperly)
		{
			CRC64 crc;

			const std::vector<int> v{ { 1, 2, 3, 4 } };

			crc.accumulate(v.begin(), v.end());

			Assert::AreEqual(crc.get(), CRC64(v).get());
		}


		TEST_METHOD(CRC64IsConsistentAcrossAccumulationTypes)
		{
			constexpr crc64_t target(3118128885020022634);

			Assert::AreEqual(target, CRC64("TEST").get());
			Assert::AreEqual(target, CRC64("TE").accumulate('S').accumulate(std::vector<char>{ {'T'} }).get());
			Assert::AreEqual(target, CRC64(std::string("T")).accumulate('E').accumulate(std::vector<char>{ {'S', 'T'} }).get());
		}


		TEST_METHOD(CRC64SupportsChaining)
		{
			CRC64 crc("TEST");
			crc.accumulate(42);
			crc.accumulate(24.0);

			Assert::AreEqual(crc.get(), CRC64("TEST").accumulate(42).accumulate(24.0).get());
		}
	};


	TEST_CLASS(CRC32Test)
	{
		TEST_METHOD(CRC32IsProperlyConstructedEmpty)
		{
			const CRC32 crc;

			Assert::AreEqual(crc32_t(0), crc.get());
			Assert::AreEqual(4ULL, sizeof(crc.get()));
		}


		TEST_METHOD(CRC32IsProperlyConstructedWithParam)
		{
			Assert::AreEqual(crc32_t(1970407491), CRC32(22).get());

			Assert::AreEqual(crc32_t(3187591094), CRC32(-112).get());

			Assert::AreEqual(crc32_t(1303229026), CRC32(-112LL).get());

			Assert::AreEqual(crc32_t(38996680), CRC32(58.12f).get());

			Assert::AreEqual(crc32_t(3707466308), CRC32(58.12).get());

			Assert::AreEqual(crc32_t(3484306596), CRC32("TEST").get());
			Assert::AreEqual(crc32_t(3484306596), CRC32(std::string("TEST")).get());
			Assert::AreEqual(crc32_t(3484306596), CRC32(std::vector<char>{ {'T', 'E', 'S', 'T'} }).get());
		}


		TEST_METHOD(CRC32AccumulatesStringsProperly)
		{
			CRC32 crc;

			crc.accumulate(std::string("TEST"));

			Assert::AreEqual(crc32_t(3484306596), crc.get());
		}


		TEST_METHOD(CRC32AccumulatesCharArraysProperly)
		{
			CRC32 crc;

			crc.accumulate("TEST");

			Assert::AreEqual(crc32_t(3484306596), crc.get());
		}


		TEST_METHOD(CRC32AccumulatesVectorsProperly)
		{
			CRC32 crc;

			crc.accumulate(std::vector<char>{ {'T', 'E', 'S', 'T'} });

			Assert::AreEqual(crc32_t(3484306596), crc.get());
		}


		TEST_METHOD(CRC32AccumulatesArithmeticTypesProperly)
		{
			CRC32 crc;

			crc.accumulate('a');
			crc.accumulate(-1);
			crc.accumulate(25LL);
			crc.accumulate(12.55f);
			crc.accumulate(1e-13);

			Assert::AreEqual(crc32_t(488281738), crc.get());
		}


		TEST_METHOD(CRC32AccumulatesSlicesProperly)
		{
			CRC32 crc;

			crc.accumulate("LKJTESTMLK", 3, 4);

			Assert::AreEqual(crc32_t(3484306596), crc.get());
		}


		TEST_METHOD(CRC32AccumulatesRandomIbjectsProperly)
		{
			class X
			{
			public:
				X(int a, double b) : mA(a), mB(b) {}

			private:
				int mA;
				double mB;
			};

			const X x(42, 42.0);
			CRC32 crc;

			crc.accumulate(reinterpret_cast<char const*>(&x), 0, sizeof(int));

			Assert::AreEqual(crc.get(), CRC32(42).get());
		}


		TEST_METHOD(CRC32AccumulatesContainersProperly)
		{
			CRC32 crc;

			const std::vector<int> v{ { 1, 2, 3, 4 } };

			crc.accumulate(v.begin(), v.end());

			Assert::AreEqual(crc.get(), CRC32(v).get());
		}


		TEST_METHOD(CRC32IsConsistentAcrossAccumulationTypes)
		{
			constexpr crc32_t target(3484306596);

			Assert::AreEqual(target, CRC32("TEST").get());
			Assert::AreEqual(target, CRC32("TE").accumulate('S').accumulate(std::vector<char>{ {'T'} }).get());
			Assert::AreEqual(target, CRC32(std::string("T")).accumulate('E').accumulate(std::vector<char>{ {'S', 'T'} }).get());
		}


		TEST_METHOD(CRC32SupportsChaining)
		{
			CRC32 crc("TEST");
			crc.accumulate(42);
			crc.accumulate(24.0);

			Assert::AreEqual(crc.get(), CRC32("TEST").accumulate(42).accumulate(24.0).get());
		}
	};
}

