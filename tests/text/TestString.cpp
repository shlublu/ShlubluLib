#define NOMINMAX

#include "CppUnitTest.h"

#include <shlublu/text/String.h>
#include <shlublu/util/Debug.h>
SHLUBLU_OPTIMIZE_OFF();


using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace shlublu;

namespace text_String
{
	TEST_CLASS(xtosTest)
	{
	public:
		TEST_METHOD(xtosDisplaysProperValues)
		{
			Assert::AreEqual(std::string("1"), String::xtos(true));
			Assert::AreEqual(std::string("50"), String::xtos(50));
			Assert::AreEqual(std::string("50.01"), String::xtos(50.01f));
			Assert::AreEqual(std::string("50.0001"), String::xtos(50.0001));
			Assert::AreEqual(std::string("1.23457e+16"), String::xtos(12345678987654321.0));
			Assert::AreEqual(std::string("1e+20"), String::xtos(1e+20));
			Assert::AreEqual(std::string("00000000DEADBEEF"), String::xtos(reinterpret_cast<void*>(static_cast<ptrdiff_t>(0xdeadbeef))));
		}


		TEST_METHOD(xtofsDisplaysProperValues)
		{
			Assert::AreEqual(std::string("50.009998"), String::xtofs(50.01f));
			Assert::AreEqual(std::string("50.000100"), String::xtofs(50.0001));
			Assert::AreEqual(std::string("12345678987654320.000000"), String::xtofs(12345678987654321.0));
			Assert::AreEqual(std::string("100000000000000000000.000000"), String::xtofs(1e+20));
		}
	};


	TEST_CLASS(splitTest)
	{
	public:
		TEST_METHOD(splitWithGivenVectorReturnsProperValues)
		{
			std::vector<std::string> splitted;
			String::split("ab,cde,fg", ',', splitted);

			Assert::AreEqual(size_t(3), splitted.size());
			Assert::AreEqual(std::string("ab"), splitted[0]);
			Assert::AreEqual(std::string("cde"), splitted[1]);
			Assert::AreEqual(std::string("fg"), splitted[2]);
		}

		TEST_METHOD(splitInlineReturnsProperValues)
		{
			const auto splitted(String::split("ab,cde,fg", ','));

			Assert::AreEqual(size_t(3), splitted.size());
			Assert::AreEqual(std::string("ab"), splitted[0]);
			Assert::AreEqual(std::string("cde"), splitted[1]);
			Assert::AreEqual(std::string("fg"), splitted[2]);
		}


		TEST_METHOD(splitWithGivenVectorClearsTheVector)
		{
			std::vector<std::string> splitted;
			splitted.push_back("DUMMY");
			String::split("ab,cde,fg", ',', splitted);

			Assert::AreEqual(size_t(3), splitted.size());
			Assert::AreEqual(std::string("ab"), splitted[0]);
			Assert::AreEqual(std::string("cde"), splitted[1]);
			Assert::AreEqual(std::string("fg"), splitted[2]);
		}
	};


	TEST_CLASS(trimTest)
	{
	public:
		TEST_METHOD(ltrimReturnsProperValues)
		{
			const std::string ref("xxx  \f\n\r\t\v  ");
			std::string test("  \f\n\r\t\v  " + ref);

			Assert::IsTrue(&test == &String::ltrim(test));
			Assert::AreEqual(ref, test);
		}


		TEST_METHOD(rtrimReturnsProperValues)
		{
			const std::string ref("  \f\n\r\t\v  xxx");
			std::string test(ref + "  \f\n\r\t\v  ");

			Assert::IsTrue(&test == &String::rtrim(test));
			Assert::AreEqual(ref, test);
		}


		TEST_METHOD(trimReturnsProperValues)
		{
			const std::string ref("xxx");
			std::string test("  \f\n\r\t\v  " + ref + "  \f\n\r\t\v  ");

			Assert::IsTrue(&test == &String::trim(test));
			Assert::AreEqual(ref, test);
		}
	};


	TEST_CLASS(replaceTest)
	{
	public:
		TEST_METHOD(replaceWorksProperlyWithRegularValues)
		{
			std::string test("once upon a time, once was meaning once");

			Assert::IsTrue(&test == &String::replace(String::replace(test, "once", "twice"), "a time", "two times"));
			Assert::AreEqual(std::string("twice upon two times, twice was meaning twice"), test);
		}


		TEST_METHOD(replaceWorksProperlyWithNothingToDo)
		{
			std::string test("once upon a time, once was meaning once");

			Assert::AreEqual(std::string("once upon a time, once was meaning once"), String::replace(test, "twice", "four times"));
		}


		TEST_METHOD(replaceWorksProperlyWithNullReplacementValue)
		{
			std::string test("once upon a time, once was meaning once");

			Assert::AreEqual(std::string("upon a time, was meaning once"), String::replace(test, "once ", ""));
		}


		TEST_METHOD(replaceWorksProperlyWithReplacementValueContainingSeed)
		{
			std::string test("once upon a time, once was meaning once");

			Assert::AreEqual(std::string("oonce upoon a time, oonce was meaning oonce"), String::replace(test, "o", "oo"));
		}


		TEST_METHOD(replaceWorksProperlyWithNullTargetValue)
		{
			std::string test("");

			Assert::AreEqual(std::string(""), String::replace(test, "once", "twice"));
		}


		TEST_METHOD(replaceThrowsWithNullSeed)
		{
			std::string test("once upon a time, once was meaning once");

			Assert::ExpectException<std::invalid_argument>([&test]() { String::replace(test, "", "anything"); });
		}
	};


	TEST_CLASS(lowerTest)
	{
	public:
		TEST_METHOD(lowerWorksPrperlyOnAMutableString)
		{
			std::string test("xXxX 123 !*%");
			auto const& ref(String::lower(test));

			Assert::AreEqual(std::string("xxxx 123 !*%"), ref);
			Assert::IsTrue(&test == &ref);
		}


		TEST_METHOD(lowerWorksPrperlyOnAnImmutableString)
		{
			const std::string test("xXxX 123 !*%");
			const auto lowered(String::lower(test));

			Assert::AreEqual(std::string("xxxx 123 !*%"), lowered);
		}
	};


	TEST_CLASS(upperTest)
	{
	public:
		TEST_METHOD(upperWorksPrperlyOnAMutableString)
		{
			std::string test("xXxX 123 !*%");
			auto const& ref(String::upper(test));

			Assert::AreEqual(std::string("XXXX 123 !*%"), ref);
			Assert::IsTrue(&test == &ref);
		}


		TEST_METHOD(upperWorksPrperlyOnAnImmutableString)
		{
			const std::string test("xXxX 123 !*%");
			const auto uppered(String::upper(test));

			Assert::AreEqual(std::string("XXXX 123 !*%"), uppered);
		}
	};


	TEST_CLASS(toWStringTest)
	{
	public:
		TEST_METHOD(toWStringReturnsIsoStringForAscii7)
		{
			const std::wstring ref(L"123AZERTabcdef!%*/\\");
			std::string src("123AZERTabcdef!%*/\\");

			Assert::AreEqual(ref, String::toWString(src));
		}
	};


	TEST_CLASS(fromWStringTest)
	{
	public:
		TEST_METHOD(fromWStringReturnsIsoStringForAscii7)
		{
			const std::string ref("123AZERTabcdef!%*/\\");
			std::wstring src(L"123AZERTabcdef!%*/\\");

			Assert::AreEqual(ref, String::fromWString(src));
		}
	};


	TEST_CLASS(levenshteinDistanceTest)
	{
	public:
		TEST_METHOD(levenshteinDistanceIcCorrect)
		{
			const std::string ref("This is a test sentence that we will use for testing.");

			Assert::AreEqual(size_t(0), String::levenshteinDistance(ref, ref));

			Assert::AreEqual(size_t(1), String::levenshteinDistance(ref	     , ref + "."));
			Assert::AreEqual(size_t(1), String::levenshteinDistance(ref + ".", ref	    ));
			Assert::AreEqual(size_t(1), String::levenshteinDistance(ref + ".", ref + "!"));

			Assert::AreEqual(size_t(2), String::levenshteinDistance(	  ref	   , "y" + ref + "."));
			Assert::AreEqual(size_t(2), String::levenshteinDistance("X" + ref      , "y" + ref + "."));
			Assert::AreEqual(size_t(2), String::levenshteinDistance("X" + ref + "Y", "z" + ref + "t"));

			Assert::AreEqual(size_t(3), String::levenshteinDistance(	  ref + ref	     , "z" + ref + "t" + ref + "."));
			Assert::AreEqual(size_t(3), String::levenshteinDistance("X" + ref + ref      , "z" + ref + "t" + ref + "."));
			Assert::AreEqual(size_t(3), String::levenshteinDistance("X" + ref + ref + "Y", "z" + ref + "t" + ref + "."));

			Assert::AreEqual(ref.length(), String::levenshteinDistance(ref + ref, ref));

			const auto countChar([&ref](char target) -> size_t { return std::count_if(ref.begin(), ref.end(), [target](char c) { return (c == target); }); });

			Assert::AreEqual(countChar('t'), String::levenshteinDistance(String::replace(std::string(ref), "t", "u"), ref));
			Assert::AreEqual(countChar('t'), String::levenshteinDistance(String::replace(std::string(ref), "t", "tt"), ref));
			Assert::AreEqual(countChar('t'), String::levenshteinDistance(ref, String::replace(std::string(ref), "t", "tt")));
		}
	}; 
}

