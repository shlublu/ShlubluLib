#pragma once

/** @file 
	Cyclic Redundancy Check.

	See CRC class documentation for details.
*/

#include <cstring>
#include <type_traits>
#include <vector>


namespace shlublu
{

/** @namespace shlublu::CRCData
	CRC tables by handled types.
*/
namespace CRCData
{
	extern "C" const uint32_t crc32Table[]; /**< 32 bits table. */
	extern "C" const uint64_t crc64Table[]; /**< 64 bits table. */
}


/**
	CRC accumulator.
	Handled types are `shlublu::crc32_t` and `shlublu::crc64_t`, which are both unsigned integers.
	Shorthands for these types are `shlublu::CRC32` and `shlublu::CRC64`.
*/ 
template <typename T> 
class CRC
{
public:
	/**
		Constructor.
		Initializes to zero.
	*/
	CRC()	
	: mHash(0)
	{
		static_assert_valid_base_type();
	}


	/**
		Constructor.
		Accumulates the passed object.
		@param param initial object to accumulate.
	*/
	template <typename P> CRC(P param) 
	: CRC()
	{ 
		static_assert_valid_base_type();
		accumulate(param); 
	}


	/**
		Accumulates a string as a series of bytes.

		@param str the string to accumulate
		@return a reference to this CRC object
	*/
	CRC<T>& accumulate(std::string const & str)
	{
		return accumulate(str.c_str(), 0, str.length());
	}


	/**
		Accumulates a C-string as a series of bytes.

		@param sz the C-string to accumulate
		@return a reference to this CRC object
	*/
	CRC<T>& accumulate(char const * sz)
	{
		return accumulate(sz, 0, ::strlen(sz));
	}


	/**
		Accumulates a vector as a series of contained objects.

		@param v the vector to accumulate
		@return a reference to this CRC object
	*/
	template <typename P> 
	CRC<T>& accumulate(std::vector<P> const & v)
	{
		accumulate(v.begin(), v.end());

		return *this;
	}



	/**
		Accumulates an arithmetic value.
		Eligibility of the parameter is evaluated at compile-time by `std::is_arithmetic<P>`.

		@param value the value to accumulate
		@return a reference to this CRC object
		@see <a href="https://www.cplusplus.com/reference/type_traits/is_arithmetic/">std::is_arithmetic</a>
	*/
	template <typename P>
	CRC<T>& accumulate(P value)
	{
		static_assert_valid_parameter_type<P>();
		return accumulate(reinterpret_cast<char const *>(&value), 0, sizeof(P));
	}


	/**
		Accumulates arbitraty bytes.
		This method can be used to accumulate an arbitrary object for which no other method applies. 
		It is up to the developper to ensure such an accumulation is valid, consistent and repeatable.

		@param data data as an array of `char`
		@param offset the offset to start accumulation from
		@param length the number of bytes to accumulate
		@return a reference to this CRC object

		<b>Example</b>
		@code
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

		crc.accumulate(reinterpret_cast<char const*>(&x), 0, sizeof(int)); // accumulates x.mA
		@endcode
	*/
	CRC<T>& accumulate(char const* data, size_t offset, size_t length)
	{
		T const * const crcTab(crcTable());
		
		// Compute the CRC
		const size_t afterLastOffset(offset + length);

		for (size_t i = offset; i < afterLastOffset; ++i)
		{
			mHash = crcTab[(mHash ^ data[i]) & 0xff] ^ (mHash >> 8);
		}

		mHash = mHash & T(-1);

		return *this;
	}


	/**
		Accumulates a range of elements.
		The range is delimited by [first,last).

		@param first iterator that begins the range
		@param last iterator that ends the range
		@return a reference to this CRC object

		<b>Example</b>
		@code
		const std::vector<int> v{ { 1, 2, 3, 4 } };
		CRC64 crc;

		crc.accumulate(v.begin(), v.end());
		@endcode
	*/
	template <typename Iter>
	CRC<T>& accumulate(const Iter first, const Iter last)
	{
		for (Iter it = first; it != last; ++it)
		{
			accumulate(*it);
		}

		return *this;
	}


	/**
		Returns the CRC value resulting from the accumulation.
		@return the CRC value
	*/
	T get() const { return mHash; }


private:
	/// @cond INTERNAL

	void static_assert_valid_base_type() const
	{
		static_assert(std::is_same<T, uint32_t>::value || std::is_same<T, uint64_t>::value, "Type should be unsigned 32 or 64.");
	}


	template<typename P> 
	void static_assert_valid_parameter_type() const
	{
		static_assert(std::is_arithmetic<P>::value, "Type should be arithmetic.");
	}


	T const * crcTable() const
	{
		return std::is_same<T, uint32_t>::value ?
			reinterpret_cast<T const *>(CRCData::crc32Table) :
			reinterpret_cast<T const *>(CRCData::crc64Table);
	}


private:
	T mHash;

	///@endcond
};


/**
	The 32 bits CRC value type.
*/
using crc32_t = uint32_t;

/**
	The 64 bits CRC value type.
*/
using crc64_t = uint64_t;

/**
	The 32 bits CRC accumulator.
*/
using CRC32 = CRC<crc32_t>;

/**
	The 64 bits CRC value type.
*/
using CRC64 = CRC<crc64_t>;

}
