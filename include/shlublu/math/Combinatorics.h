#pragma once

#include <algorithm>
#include <vector>

#include <shlublu/math/Math.h>
#include <shlublu/text/String.h>

/** @file
	Combinatorics-oriented classes.

	See classes documentation for details.
*/


namespace shlublu
{


template <typename T>
class EnumerativeCombinatorics
{
public:
	EnumerativeCombinatorics(size_t k)
		: mKUplet()
	{
		mKUplet.reserve(k);
	}

	virtual ~EnumerativeCombinatorics()
	{}

	virtual size_t n() const = 0;
	virtual size_t k() const = 0;
	
	virtual size_t number() const = 0;

	std::vector<T> const& kUplet() const
	{
		return mKUplet;
	}

	void swapKUplet(std::vector<T>& swapWith) 
	{
		mKUplet.swap(swapWith);
	}

	virtual bool next() = 0;

protected:
	std::vector<T> mKUplet;
};


template <typename T>
class Combination : public EnumerativeCombinatorics<T>
{
public:
	Combination(size_t n, size_t k)
		: EnumerativeCombinatorics<T>(k),
		  mN(n),
		  mK(k),
		  mBitmask(k, 1), // K leading 1's
		  mNextAvailable(k > 0)
	{
		static_assert(std::is_arithmetic<T>::value && !std::is_same<T, bool>::value, "Type should be non-boolean arithmetic");

		if (mK > mN)
		{
			throw std::invalid_argument("Combinations<T>::Combinations(): k > n: " + shlublu::String::xtos(mK) + " > " + shlublu::String::xtos(mN));
		}

		mBitmask.resize(n, 0); // N-K trailing 0's
	}


	virtual ~Combination()
	{}


	virtual size_t n() const 
	{ 
		return mN; 
	}


	virtual size_t k() const 
	{ 
		return mK; 
	}


	virtual size_t number() const
	{
		return number(mN, mK);
	}


	virtual bool next()
	{
		const bool ret(mNextAvailable);

		if (ret)
		{
			EnumerativeCombinatorics<T>::mKUplet.clear();

			for (size_t i = 0; i < mN && EnumerativeCombinatorics<T>::mKUplet.size() < mK; ++i) // [0..N-1] integers
			{
				if (mBitmask[i])
				{
					EnumerativeCombinatorics<T>::mKUplet.push_back(T(i));
				}
			}

			mNextAvailable = std::prev_permutation(mBitmask.begin(), mBitmask.end());
		}

		return ret;
	}


	static size_t number(size_t n, size_t k)
	{
		return (shlublu::Math::factorial(n) / shlublu::Math::factorial(n - k)) / shlublu::Math::factorial(k);
	}


private:
	const size_t mN;
	const size_t mK;

	std::string mBitmask;
	bool mNextAvailable;
};


template <typename T>
class Arrangement : public EnumerativeCombinatorics<T>
{
public:
	Arrangement(size_t n, size_t k)
		:  EnumerativeCombinatorics<T>(k),
		   mCombinations(n, k),
		   mStarted(false)
	{}


	virtual ~Arrangement()
	{}


	virtual bool next()
	{
		bool ret(mStarted);

		if (ret)
		{
			ret = std::next_permutation(EnumerativeCombinatorics<T>::mKUplet.begin(), EnumerativeCombinatorics::mKUplet.end());
		}

		if (!ret)
		{
			mStarted = true;

			mCombinations.swapKUplet(EnumerativeCombinatorics<T>::mKUplet);
			ret = mCombinations.next();
			mCombinations.swapKUplet(EnumerativeCombinatorics<T>::mKUplet);
		}

		return ret;
	}


	virtual size_t n() const 
	{ 
		return mCombinations.n(); 
	}


	virtual size_t k() const 
	{ 
		return mCombinations.k(); 
	}


	virtual size_t number() const
	{
		return number(n(), k());
	}


	static size_t number(size_t n, size_t k)
	{
		return shlublu::Math::factorial(n) / shlublu::Math::factorial(n - k);
	}


private:
	Combination<T> mCombinations;
	bool mStarted;
};


}
