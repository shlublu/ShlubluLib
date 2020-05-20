#pragma once

#include <random>

#include <shlublu/text/String.h>
#include <shlublu/util/Exceptions.h>

/** @file
	Helper functions based on [`std::mt19937_64`](https://www.cplusplus.com/reference/random/mt19937_64/) engine.

	See Random namespace documentation for details.
*/


namespace shlublu
{

namespace Random
{
	using Engine = std::mt19937_64;

	/// @cond INTERNAL 
	extern Engine* __sRandomEngine;
	/// @endcond


	// Basic min-max random
	template<typename T> T random(T min, T max)
	{
		if (min > max)
		{
			throw ShlubluException("Random::random(): " + String::xtos(min) + " >= " + String::xtos(max));
		}

		using Distribution = typename std::conditional<std::is_floating_point<T>::value, std::uniform_real_distribution<T>, std::uniform_int_distribution<T>>::type;
		return Distribution{ min, max }(*__sRandomEngine);
	}


	// Rounded random
	template<typename T> T randomRounded(T min, T max, T step)
	{
		static_assert(std::is_arithmetic<T>::value, "Type should be arithmetic.");

		if (step < 0)
		{
			throw ShlubluException("Random::randomRounded(): " + String::xtos(step) + " is negative.");
		}

		T ret;

		if (std::is_integral<T>::value)
		{
			ret = static_cast<T>(randomRounded<double>(min, max, step));
		}
		else if (std::is_floating_point<T>::value)
		{
			// Round it to the nearest step
			const T multiplier(T(1) / step);

			ret = static_cast<T>(std::round(random(min, max) * multiplier) / multiplier);
		}

		return ret;
	}


	// Unit random
	template<typename T> T randomUnit()
	{
		return random<T>(T(0), T(1));
	}


	template<typename T> T randomRelativeUnit()
	{
		static_assert(std::is_signed<T>::value, "Type should be signed.");

		return random<T>(T(-1), T(1));
	}


	// Probabilities
	template<typename T> bool probability(T p)
	{
		static_assert(std::is_floating_point<T>::value, "Type should be floating point.");

		if (p < T(0))
		{
			throw ShlubluException("Random::probability(): " + String::xtos(p) + " is negative.");
		}
		else if (p > T(1))
		{
			throw ShlubluException("Random::probability(): " + String::xtos(p) + " > 1");
		}

		bool ret(p == T(1));

		if (!ret && p != T(0))
		{
			ret = randomUnit<T>() < p;
		}

		return ret;
	}


	template<typename T> bool likelihood(T chances, T inTotal)
	{
		static_assert(std::is_integral<T>::value && !std::is_signed<T>::value, "Type should be unsigned integral.");

		if (chances > inTotal)
		{
			throw ShlubluException("Random::likelihood(): " + String::xtos(chances) + " > " + String::xtos(inTotal));
		}

		return random(T(1), inTotal) <= chances;
	}


	bool tossACoin();


	// Normal distribution random
	template<typename T> T randomNormalDist(std::normal_distribution<T>& dist)
	{
		return dist(*__sRandomEngine);
	}
}

}
