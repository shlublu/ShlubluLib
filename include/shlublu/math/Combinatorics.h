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


class EnumerativeCombinatorics
{
public:
	EnumerativeCombinatorics(size_t k)
		: mK(k),
		  mKUplet()
	{
		mKUplet.reserve(mK);
	}

	virtual ~EnumerativeCombinatorics()
	{}

	virtual size_t n() const = 0;
	
	size_t k() const
	{
		return mK;
	}

	virtual size_t number() const = 0;

	std::vector<size_t> const& kUplet() const
	{
		return mKUplet;
	}


private:
	friend class Arrangement;

	void swapKUplet(std::vector<size_t>& swapWith)
	{
		mKUplet.swap(swapWith);
	}

	virtual bool next() = 0;

protected:
	const size_t mK;

	std::vector<size_t> mKUplet;
};


class Combination : public EnumerativeCombinatorics
{
public:
	Combination(size_t n, size_t k)
		: EnumerativeCombinatorics(k),
		  mN(n),
		  mBitmask(k, 1), // K leading 1's
		  mNextAvailable(k > 0)
	{
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


	virtual size_t number() const
	{
		return number(mN, k());
	}


	virtual bool next()
	{
		const bool ret(mNextAvailable);

		if (ret)
		{
			mKUplet.clear();

			for (size_t i = 0; i < mN && mKUplet.size() < k(); ++i) // [0..N-1] integers
			{
				if (mBitmask[i])
				{
					mKUplet.push_back(i);
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

	std::string mBitmask;
	bool mNextAvailable;
};


class Arrangement : public EnumerativeCombinatorics
{
public:
	Arrangement(size_t n, size_t k)
		:  EnumerativeCombinatorics(k),
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
			ret = std::next_permutation(mKUplet.begin(), mKUplet.end());
		}

		if (!ret)
		{
			mStarted = true;

			mCombinations.swapKUplet(mKUplet);
			ret = mCombinations.next();
			mCombinations.swapKUplet(mKUplet);
		}

		return ret;
	}


	virtual size_t n() const 
	{ 
		return mCombinations.n(); 
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
	Combination mCombinations;
	bool mStarted;
};


}
