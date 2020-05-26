#pragma once

#include <algorithm>
#include <cmath> 
#include <functional>

#include <shlublu/text/String.h>
#include <shlublu/util/Exceptions.h>


namespace shlublu
{

namespace Math
{
	// Constants
	template<typename T> inline T constantE() { return T(2.71828182845904523536); } // e
	template<typename T> inline T constantLog2e() { return T(1.44269504088896340736); } // log2(e)
	template<typename T> inline T constantLog10e() { return T(0.434294481903251827651); } // log10(e)
	template<typename T> inline T constantLn2() { return T(0.693147180559945309417); }  // ln(2)
	template<typename T> inline T constantLn10() { return T(2.30258509299404568402); } // ln(10)
	template<typename T> inline T constantPi() { return T(3.14159265358979323846); } // pi
	template<typename T> inline T constantPiBy2() { return T(1.57079632679489661923); } // pi/2
	template<typename T> inline T constantPiBy4() { return T(0.785398163397448309616); } // pi/4
	template<typename T> inline T constant1ByPi() { return T(0.318309886183790671538); } // 1/pi
	template<typename T> inline T constant2ByPi() { return T(0.636619772367581343076); } // 2/pi
	template<typename T> inline T constant2BySqrtPi() { return T(1.12837916709551257390); } // 2/sqrt(pi)
	template<typename T> inline T constantSqrt2() { return T(1.41421356237309504880); } // sqrt(2)
	template<typename T> inline T constant1BySqrt2() { return T(0.707106781186547524401); } // 1/sqrt(2)


	template<typename T> inline T constantMaxSafeInteger()
	{
		// Returns the max double that can be incremented to give a different value (all floating types not specialized below)
		static_assert(std::is_floating_point<T>::value, "Type should be floating point.");
		return 9007199254740991.0; /* 'long double' is actually higher under Linux but M_MAXSAFEINTEGER_DOUBLE is used anyway, currently. */
	}
	

	template<> inline float constantMaxSafeInteger()
	{
		// Returns the max float that can be incremented to give a different value (float)
		return 16777215.0f;
	}


	template<typename T> inline T constantMinSafeInteger()
	{
		// Returns the min integer that can be decremented to give a different value (all floating types)
		static_assert(std::is_floating_point<T>::value, "Type should be floating point.");
		return -constantMaxSafeInteger<T>();
	}

	   
	// Factorial
	template<typename T> T factorial(T n)
	{
		if (n < T(0))
		{
			throw ShlubluException("Math::factorial(): " + String::xtos(n) + " is negative.");
		}

		if (std::is_floating_point<T>::value && std::round(n) != n)
		{
			throw ShlubluException("Math::factorial(): " + String::xtos(n) + " is not round.");
		}

		std::function<T(T)> factorialUnchecked =
		(
			[&factorialUnchecked](T n) -> T
			{
				return (n > T(1)) ? (factorialUnchecked(n - T(1)) * n) : T(1);
			}
		);

		return factorialUnchecked(n);
	}


	// Clamp
	template<typename T> T clamp(T value, T minVal, T maxVal)
	{
		if (minVal > maxVal)
		{
			throw ShlubluException("Math::clamp(): " + String::xtos(minVal) + " > " + String::xtos(maxVal));
		}

		const T upperBound(std::min<T>(value, maxVal));
		return std::max<T>(minVal, upperBound);
	}


	// Round
	template<typename T> T roundX(T value, unsigned pow10)
	{
		static_assert(std::is_floating_point<T>::value, "Type should be floating point.");
		
		const T factor(std::pow(T(10.0), static_cast<T>(pow10)));

		return std::round(factor * value) / factor;
	}


	template<typename T> T round2(T value)
	{
		return roundX(value, 2);
	}


	// Percentile
	template<typename T> inline T increase(T initialValue, T finalValue)
	{
		static_assert(std::is_floating_point<T>::value, "Type should be floating point.");

		if (initialValue == 0)
		{
			throw ShlubluException("Math::increase(): initialValue (" + String::xtos(initialValue) + ") should not be zero.");
		}

		return (finalValue / initialValue) - T(1.0);
	}


	template<typename T> inline T increaseRate(T overallIncrease, size_t numIterations)
	{
		// https://pages.uoregon.edu/rgp/PPPM613/class8a.htm
		static_assert(std::is_floating_point<T>::value, "Type should be floating point.");

		if (numIterations == 0)
		{
			throw ShlubluException("Math::increaseRate(): numIterations (" + String::xtos(numIterations) + ") should be strictly positive.");
		}

		if (overallIncrease <= -1.0)
		{
			throw ShlubluException("Math::increaseRate(): overallIncrease (" + String::xtos(overallIncrease) + ") should be greater than -1.0.");
		}

		return std::pow<T>(T(1.0) + overallIncrease, T(1.0) / numIterations) - T(1.0);
	}


	template<typename T> inline T increaseRate(T initialValue, T finalValue, size_t numIterations)
	{
		return increaseRate(increase(initialValue, finalValue), numIterations);
	}	


	// Sign
	template<typename T> bool sameSign(T x, T y)
	{
		return (x >= T(0)) ^ (y < T(0));
	}
}

}
