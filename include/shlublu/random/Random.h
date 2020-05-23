#pragma once

#include <random>

#include <shlublu/text/String.h>
#include <shlublu/util/Exceptions.h>

/** @file
	Helper functions based on the standard <a href="https://www.cplusplus.com/reference/random/">random</a> header.

	See Random namespace documentation for details.
*/


namespace shlublu
{

/**
	@namespace shlublu::Random
	Set of helper functions intended to make usual operations easier.
	These functions are based on the standard <a href="https://www.cplusplus.com/reference/random/">random</a> header and use a 
	<a href="https://www.cplusplus.com/reference/random/mt19937_64/">`std::mt19937_64`</a> engine. 
	Distribution is <a href="https://www.cplusplus.com/reference/random/uniform_real_distribution/">std::uniform_real_distribution</a> for real types and
	<a href="https://www.cplusplus.com/reference/random/uniform_int_distribution/">std::uniform_int_distribution</a> for integral types.

	The engine is initialized automatically. There is no need (and no way) to seed it with a timer or anything.
*/
namespace Random
{
	using Engine = std::mt19937_64; /**< Engine that is used by all Random functions. */

	/// @cond INTERNAL 
	extern Engine* __sRandomEngine;
	/// @endcond


	/**
		Returns a number from a given range.
		The returned number belongs to the following range:
		<ul>
		<li>[`min`, `max`) for real numbers</li>
		<li>[`min`, `max`] for integers</li>
		</ul>

		@tparam T the type of `min`, `max` and the returned value. This type should be arithmetic.
		@param min the minimal value of the range
		@param max the maximal value of the range, included for integers and excluded for real numbers. It should be strictly greater than `min`.
		@return a pseudo-random number in the range defined by `min` and `max`
		@exception ShlubluException if `min` >= `max`

		<b>Example</b>
		@code
		std::cout << Random::random(-5.0, 5.0) << std::endl;  // For example: 2.58067
		@endcode
	*/
	template<typename T> T random(T min, T max)
	{
		static_assert(std::is_arithmetic<T>::value, "Type should be arithmetic.");

		if (min >= max)
		{
			throw ShlubluException("Random::random(): min (" + String::xtos(min) + ") >= max (" + String::xtos(max) + ")");
		}

		using Distribution = typename std::conditional<std::is_floating_point<T>::value, std::uniform_real_distribution<T>, std::uniform_int_distribution<T>>::type;

		return Distribution{ min, max }(*__sRandomEngine);
	}


	/**
		Returns a number from a given range and rounded following a given step.
		The returned number belongs to the following range:
		<ul>
		<li>[`min`, `max`) for real numbers</li>
		<li>[`min`, `max`] for integers</li>
		</ul>
		It is rounded to the nearest step.

		@tparam T the type of `min`, `max`, `step` and the returned value. This type should be arithmetic.
		@param min the minimal value of the range
		@param max the maximal value of the range, included for integers and excluded for real numbers. It should be strictly greater than `min`.
		@param step the rounding step. It must be lower than the amplitude of the range.
		@return a pseudo-random number in the range defined by `min` and `max` and rounded to the nearest `step`.
		@exception ShlubluException if `step` is negative or null, if `step` is greater than the amplitude of the range, or if `min` >= `max`

		<b>Example</b>
		@code
		std::cout << Random::random(-5.0, 5.0, 0.25) << std::endl; // For example: -3.75
		@endcode
	*/
	template<typename T> T random(T min, T max, T step)
	{
		static_assert(std::is_arithmetic<T>::value, "Type should be arithmetic.");

		if (step <= 0)
		{
			throw ShlubluException("Random::random(): step (" + String::xtos(step) + ") is negative or null.");
		}

		if (step > (max - min))
		{
			throw ShlubluException("Random::random(): step (" + String::xtos(step) + ") is bigger than the amplitude of the range (" + String::xtos(max - min) + ").");
		}

		T ret;

		if (std::is_integral<T>::value)
		{
			ret = static_cast<T>(random<double>(min, max, step));
		}
		else if (std::is_floating_point<T>::value)
		{
			// Rounds to the nearest step
			const T multiplier(T(1) / step);

			ret = static_cast<T>(std::round(random(min, max) * multiplier) / multiplier);
		}

		return ret;
	}


	/**
		Returns a number from a unitary range.
		Depending on `T`, the returned value:
		<ul>
		<li>is an element of [0, 1) for real numbers</li>
		<li>is either 0 or 1 for integers</li>
		</ul>

		@tparam T the type of the returned value. This type should be arithmetic.
		@return a pseudo-random number in the range defined by `0` and `1`

		<b>Example</b>
		@code
		std::cout << Random::randomUnit<double>() << std::endl; // For example: 0.579902
		@endcode	*/
	template<typename T> T randomUnit()
	{
		return random<T>(T(0), T(1));
	}


	/**
		Returns a number from a relative unitary range.
		Depending on `T`, the returned value:
		<ul>
		<li>is an element of [-1, 1) for real numbers</li>
		<li>is either -1, 0 or 1 for integers</li>
		</ul>

		@tparam T the type of the returned value. This type should be arithmetic and signed.
		@return a pseudo-random number in the range defined by `-1` and `1`

		<b>Example</b>
		@code
		std::cout << Random::randomRelativeUnit<double>() << std::endl; // For example: -0.926407
		@endcode	
	*/
	template<typename T> T randomRelativeUnit()
	{
		static_assert(std::is_signed<T>::value, "Type should be signed.");

		return random<T>(T(-1), T(1));
	}


	/**
		Returns a boolean depending on a probability.
		This function has a probability of:
		<ul>
		<li>`p` to return `true`</li>
		<li>`1 - p` ti return `false`</li>
		</ul>

		@tparam T the type of the returned value. This type should be real.
		@param p the probability of returning `true`. It should be in the range [0, 1]
		@return `true` or `false` depending on a pseudo-random number 
		@exception ShlubluException if `p` is out of the range [0, 1]

		<b>Example</b>
		@code
		std::cout << (Random::probability(0.6) ? "yes" : "no") << std::endl; // For example: "yes"
		@endcode
	*/
	template<typename T> bool probability(T p)
	{
		static_assert(std::is_floating_point<T>::value, "Type should be floating point.");

		if (p < T(0))
		{
			throw ShlubluException("Random::probability(): p (" + String::xtos(p) + ") is negative.");
		}
		else if (p > T(1))
		{
			throw ShlubluException("Random::probability(): p (" + String::xtos(p) + ") > 1");
		}

		bool ret(p == T(1));

		if (!ret && p != T(0))
		{
			ret = randomUnit<T>() < p;
		}

		return ret;
	}


	/**
		Returns a boolean depending on the likelihood of an event.
		This function has a `chance` out of `total` chance to return true. It returns `false` otherwise.

		@tparam T the type of the returned value. This type should be integral.
		@param chance the numberator of the chance
		@paral total the denominator of the chance
		@return `true` or `false` depending on a pseudo-random number 
		@exception ShlubluException if `T` is signed and `chance` is negative or if `chance` > `total`

		<b>Example</b>
		@code
		std::cout << (Random::likelihood(3, 11) ? "yes" : "no") << std::endl; // For example: "no"
		@endcode
	*/
	template<typename T> bool likelihood(T chance, T total)
	{
		static_assert(std::is_integral<T>::value, "Type should be integral.");
		static_assert(!std::is_signed<T>::value, "Type should be unsigned.");

		if (std::is_signed<T>::value && chance < 0)
		{
			throw ShlubluException("Random::likelihood(): chance (" + String::xtos(chance) + ") is negative");
		}

		if (chance > total)
		{
			throw ShlubluException("Random::likelihood(): chance (" + String::xtos(chance) + ") > total (" + String::xtos(total) + ")");
		}

		return random(T(1), total) <= chance;
	}


	/**
		Returns a boolean depending on a probability equal to 1/2.

		@return `true` or `false` depending on a pseudo-random number

		<b>Example</b>
		@code
		std::cout << (Random::tossACoin() ? "heads" : "tails") << std::endl; // For example: "heads"
		@endcode
	*/

	bool tossACoin();
}

}
