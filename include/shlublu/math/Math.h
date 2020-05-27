#pragma once

#include <algorithm>
#include <cmath> 
#include <functional>

#include <shlublu/text/String.h>
#include <shlublu/util/Exceptions.h>

/** @file
	Helper functions not included in the standard <a href="https://www.cplusplus.com/reference/cmath/">&lt;cmath&gt;</a> header.

	See Math namespace documentation for details.
*/


namespace shlublu
{

/**
	@namespace shlublu::Math
	Set of helper constants and functions.
*/
namespace Math
{
	/**
		Constant \f$e\f$.
		As per the legacy `<math.h>` C header.
		@tparam T the type of the returned value. This type should be floating point.
		@return \f$e\f$
	*/
	template<typename T> inline T constantE() 
	{ 
		static_assert(std::is_floating_point<T>::value, "Type should be floating point."); 
		return T(2.71828182845904523536); 
	} 

	/**
		Constant \f$log2(e)\f$.
		As per the legacy `<math.h>` C header.
		@tparam T the type of the returned value. This type should be floating point.
		@return \f$log2(e)\f$
	*/
	template<typename T> inline T constantLog2e()
	{ 
		static_assert(std::is_floating_point<T>::value, "Type should be floating point."); 
		return T(1.44269504088896340736); 
	} 

	/**
		Constant \f$log10(e)\f$.
		As per the legacy `<math.h>` C header.
		@tparam T the type of the returned value. This type should be floating point.
		@return \f$log10(e)\f$
	*/
	template<typename T> inline T constantLog10e()
	{ 
		static_assert(std::is_floating_point<T>::value, "Type should be floating point."); 
		return T(0.434294481903251827651); 
	} 

	/**
		Constant \f$ln(2)\f$.
		As per the legacy `<math.h>` C header.
		@tparam T the type of the returned value. This type should be floating point.
		@return \f$ln(2)\f$
	*/
	template<typename T> inline T constantLn2()
	{ 
		static_assert(std::is_floating_point<T>::value, "Type should be floating point."); 
		return T(0.693147180559945309417); 
	} 

	/**
		Constant \f$ln(10)\f$.
		As per the legacy `<math.h>` C header.
		@tparam T the type of the returned value. This type should be floating point.
		@return \f$ln(10)\f$
	*/
	template<typename T> inline T constantLn10()
	{ 
		static_assert(std::is_floating_point<T>::value, "Type should be floating point."); 
		return T(2.30258509299404568402); 
	} 

	/**
		Constant \f$\pi\f$.
		As per the legacy `<math.h>` C header.
		@tparam T the type of the returned value. This type should be floating point.
		@return \f$\pi\f$
	*/
	template<typename T> inline T constantPi()
	{ 
		static_assert(std::is_floating_point<T>::value, "Type should be floating point."); 
		return T(3.14159265358979323846); 
	} 

	/**
		Constant \f$\pi / 2\f$.
		As per the legacy `<math.h>` C header.
		@tparam T the type of the returned value. This type should be floating point.
		@return \f$\pi / 2\f$
	*/
	template<typename T> inline T constantPiBy2()
	{ 
		static_assert(std::is_floating_point<T>::value, "Type should be floating point."); 
		return T(1.57079632679489661923); 
	} 

	/**
		Constant \f$\pi / 4\f$.
		As per the legacy `<math.h>` C header.
		@tparam T the type of the returned value. This type should be floating point.
		@return \f$\pi / 4\f$
	*/
	template<typename T> inline T constantPiBy4()
	{ 
		static_assert(std::is_floating_point<T>::value, "Type should be floating point."); 
		return T(0.785398163397448309616); 
	} 

	/**
		Constant \f$1 / \pi\f$.
		As per the legacy `<math.h>` C header.
		@tparam T the type of the returned value. This type should be floating point.
		@return \f$1 / \pi\f$
	*/
	template<typename T> inline T constant1ByPi()
	{ 
		static_assert(std::is_floating_point<T>::value, "Type should be floating point."); 
		return T(0.318309886183790671538); 
	} 

	/**
		Constant \f$2 / \pi\f$.
		As per the legacy `<math.h>` C header.
		@tparam T the type of the returned value. This type should be floating point.
		@return \f$2 / \pi\f$
	*/
	template<typename T> inline T constant2ByPi()
	{ 
		static_assert(std::is_floating_point<T>::value, "Type should be floating point.");
		return T(0.636619772367581343076);
	} 

	/**
		Constant \f$2 / \sqrt{\pi}\f$.
		As per the legacy `<math.h>` C header.
		@tparam T the type of the returned value. This type should be floating point.
		@return \f$2 / \sqrt{\pi}\f$
	*/
	template<typename T> inline T constant2BySqrtPi() 
	{ 
		static_assert(std::is_floating_point<T>::value, "Type should be floating point."); 
		return T(1.12837916709551257390); 
	} 

	/**
		Constant \f$\sqrt{2}\f$.
		As per the legacy `<math.h>` C header.
		@tparam T the type of the returned value. This type should be floating point.
		@return \f$\sqrt{2}\f$
	*/
	template<typename T> inline T constantSqrt2()
	{ 
		static_assert(std::is_floating_point<T>::value, "Type should be floating point."); 
		return T(1.41421356237309504880);
	}

	/**
		Constant \f$1 / \sqrt{2}\f$.
		As per the legacy `<math.h>` C header.
		@tparam T the type of the returned value. This type should be floating point.
		@return \f$1 / \sqrt{2}\f$
	*/
	template<typename T> inline T constant1BySqrt2()
	{ 
		static_assert(std::is_floating_point<T>::value, "Type should be floating point.");
		return T(0.707106781186547524401); 
	} 


	/**
		Maximal value that can be incremented to give a different value.
		Determined as \f$f + 1 \neq f\f$.
		@tparam T the type to apply this function to. This type should be floating point.
		@return the value applicable to `T`

		@note Linux `long double` is currently handled as `double` though it would support higher values.
	*/
	template<typename T> inline T constantMaxIncrementable()
	{
		static_assert(std::is_floating_point<T>::value, "Type should be floating point.");
		return 9007199254740991.0; 
	}
	

	/// @cond INTERNAL
	template<> inline float constantMaxIncrementable()
	{
		return 16777215.0f;
	}
	/// @endcond 


	/**
		Minimal value that can be decremented to give a different value.
		Determined as \f$f - 1 \neq f\f$.
		@tparam T the type to apply this function to. This type should be floating point.
		@return the value applicable to `T`

		@note Linux `long double` is currently handled as `double` though it would support lower values.
	*/
	template<typename T> inline T constantMinDecrementable()
	{
		static_assert(std::is_floating_point<T>::value, "Type should be floating point.");
		return -constantMaxIncrementable<T>();
	}

	   
	/**
		Tells whether `x` and `y` are of the same sign.
		
		@tparam the type of `x` and `y`. This type should be arithmetic.
		@param x a value
		@param y another value
		@return `true` if `x` and `y` are of the same sign, `false` otherwise
	*/
	template<typename T> bool sameSign(T x, T y)
	{
		static_assert(std::is_arithmetic<T>::value, "Type should be arithmetic.");

		return (x >= T(0)) ^ (y < T(0));
	}


	/**
		Clamps a value to a range.
		@tparam T The type of the value and the range. This type should be non-boolean arithmetic.
		@param value the value to clamp
		@param minVal the lower inclusive bound of the range
		@param maxVal the upper inclusive bound of the range
		@return `value` if \f$value \in [minVal, maxVal]\f$, `minVal` if \f$value < minVal\f$, and `maxVal` if \f$value > maxVal\f$
		@exception ShlubluException if `minVal > maxVal`
	*/
	template<typename T> T clamp(T value, T minVal, T maxVal)
	{
		static_assert(std::is_arithmetic<T>::value && !std::is_same<T, bool>::value, "Type should be non-boolean arithmetic.");

		if (minVal > maxVal)
		{
			throw ShlubluException("Math::clamp(): " + String::xtos(minVal) + " > " + String::xtos(maxVal));
		}

		return std::max<T>(minVal, std::min<T>(value, maxVal));
	}


	/**
		Rounds a number to the specified number of fractional digits.
		@tparam T the type of the number to round and of the returned value. This type should be floating point.
		@param number the number to round
		@param digits the number of fractional digits to round `number` to
		@return the rounded value of `number`
	*/
	template<typename T> T roundX(T number, size_t digits)
	{
		static_assert(std::is_floating_point<T>::value, "Type should be floating point.");
		
		const T factor(std::pow(T(10.0), T(digits)));

		return std::round(factor * number) / factor;
	}


	/**
		Rounds a number to 2 fractional digits.
		@tparam T the type of the number to round and of the returned value. This type should be floating point.
		@param number the number to round
		@return the rounded value of `number`
		@see roundX()
	*/
	template<typename T> T round2(T value)
	{
		return roundX(value, 2);
	}


	/**
		Returns the factorial of a given number.
		@tparam T the type of `n` and of the returned value
		@param n the number whose factorial is to be calculated
		@return the factorial of `n`
		@exception ShlubluException if `n` is negative or not round
	*/
	template<typename T> T factorial(T n)
	{
		static_assert(std::is_arithmetic<T>::value && !std::is_same<T, bool>::value, "Type should be arithmetic.");

		if (n < T(0))
		{
			throw ShlubluException("Math::factorial(): " + String::xtos(n) + " is negative.");
		}

		if (std::is_floating_point<T>::value && std::round(n) != n)
		{
			throw ShlubluException("Math::factorial(): " + String::xtos(n) + " is not round.");
		}

		const std::function<T(T)> factorialUnchecked =
			(
				[&factorialUnchecked](T n) -> T
				{
					return (n > T(1)) ? (factorialUnchecked(n - T(1)) * n) : T(1);
				}
		);

		return factorialUnchecked(n);
	}


	/**
		Returns the proportional increase from an initial value to a final value.
		The increase `I` is defined as \f$Vfinal = Vinitial + (Vinitial \times I)\f$, leading to \f$I = (Vfinal / Vinitial) - 1\f$. 
		
		If `I` be negative, this is a decrease. 

		@tparam VALUES_TYPE the type of the initial and final values. This type should be arithmetic.
		@tparam INCREASE_TYPE the type of the resulting increase. This type should be floating point.
		@param initialValue the initial value
		@param finalValue the final value
		@return The proportional increase \f$(Vfinal / Vinitial) - 1\f$. `initialValue` is converted to `INCREASE_TYPE` to ensure this is a floating point division.
		@exception ShlubluException if `initialValue` is zero
	*/
	template<typename VALUES_TYPE, typename INCREASE_TYPE = VALUES_TYPE> 
	inline INCREASE_TYPE proportionalIncrease(VALUES_TYPE initialValue, VALUES_TYPE finalValue)
	{
		static_assert(std::is_arithmetic<VALUES_TYPE>::value, "VALUES_TYPE should be arithmetic.");
		static_assert(std::is_floating_point<INCREASE_TYPE>::value, "INCREASE_TYPE should be floating point.");

		if (initialValue == 0)
		{
			throw ShlubluException("Math::proportionalIncrease(): initialValue (" + String::xtos(initialValue) + ") should not be zero.");
		}

		return (finalValue / INCREASE_TYPE(initialValue)) - INCREASE_TYPE(1.0);
	}


	/**
		Converts an proportional increase over a number of periods to an increase rate per period.
		This is the increase rate that produces the overall increase when applied to each period.<br />
		This rate `R` is defined as \f$(R + 1) ^ N = O + 1\f$, where `N` is the number of periods and `O` the overall increase. This leads to \f$R = \sqrt[N]{O + 1} - 1\f$. 

		This function only applies to \f$O > -1\f$ as `-1` or less would correspond to a decrease of 100% or more, leading the result `R` not to be a real number. 

		@tparam T the type of the overall increase and the returned rate. This type should be floating point.
		@param overallIncrease the proportional increase over all the periods of times
		@param numPeriods the number of periods of time that are covered
		@return the increase rate \f$\sqrt[N]{O + 1} - 1\f$
		@exception ShlubluException if `numPeriods` is zero or if `overallIncrease <= -1`
	*/
	template<typename T> inline T increaseRate(T overallIncrease, size_t numPeriods)
	{
		static_assert(std::is_floating_point<T>::value, "Type should be floating point.");

		if (numPeriods == 0)
		{
			throw ShlubluException("Math::increaseRate(): numPeriods (" + String::xtos(numPeriods) + ") should be strictly positive.");
		}

		if (overallIncrease <= -1.0)
		{
			throw ShlubluException("Math::increaseRate(): overallIncrease (" + String::xtos(overallIncrease) + ") should be greater than -1.0.");
		}

		return std::pow<T>(T(1.0) + overallIncrease, T(1.0) / numPeriods) - T(1.0);
	}


	/**
		Returns the increase rate per period giving a final value from an initial value over a given number of periods.

		This rate `R` is defined as \f$Vfinal = Vinitial \times (R + 1)^N\f$ where `N` is the number of periods.
		This leads to \f$R = \sqrt[N]{Vfinal / Vinitial} - 1\f$.
		
		This function only applies to:
		<ul>
		<li>\f$Vinitial > 0\f$ and</li>
		<li>\f$Vfinal / Vinitial > 0\f$ as zero or less would correspond to a decrease of 100% or more, leading the result `R` not to be a real number.</li>
		</ul>

		@tparam VALUES_TYPE the type of the initial and final values. This type should be arithmetic.
		@tparam INCREASE_TYPE the type of the resulting increase. This type should be floating point.
		@param initialValue the initial value
		@param finalValue the final value
		@param numPeriods the number of periods of time that are covered
		@return the increase rate \f$\sqrt[N]{Vfinal / Vinitial} - 1\f$
		@exception ShlubluException if `initialValue` is zero, if `numPeriods` is zero or if `finalValue / initialValue <= 0`
	*/
	template<typename VALUES_TYPE, typename INCREASE_TYPE = VALUES_TYPE>
	inline INCREASE_TYPE increaseRate(VALUES_TYPE initialValue, VALUES_TYPE finalValue, size_t numPeriods)
	{
		return increaseRate(proportionalIncrease<VALUES_TYPE, INCREASE_TYPE>(initialValue, finalValue), numPeriods);
	}	
}

}
