#pragma once

/** @file
	Helper functions that are not included in <a href="https://www.cplusplus.com/reference/string/string/">std::string</a>

	See String namespace documentation for details.
*/

#include <algorithm>
#include <iomanip>
#include <sstream>
#include <vector>

namespace shlublu
{

/** @namespace String
	Helper functions for <a href="https://www.cplusplus.com/reference/string/string/">std::string</a>.
*/
namespace String
{
	/**
		Converts arithmetic values or pointers to std::string.
		@param arg the value to convert
		@return the string representation of arg

		<b>Example</b>
		@code
		const std::string valOf42(String::xtos(42));			// "42"
		const std::string valOf42AndAHalf(String::xtos(42.5));	// "42.5"
		const std::string valOfThis(this);						// for example: "000000013F75193C"
		@endcode
	 */
	template <typename T>
	inline std::string xtos(T arg)
	{
		static_assert(std::is_arithmetic<T>::value || std::is_pointer<T>::value, "Type should be either arithmetic or pointer.");

		std::ostringstream buffer;
		buffer << arg;

		return buffer.str();
	}


	/**
		Converts floating point values to std::string in fixed notation.
		@param arg the value to convert
		@return the string representation of arg in fixed notation

		<b>Example</b>
		@code
		const std::string valOfFloat42(String::xtos(42.0f));			// "42.000000"
		const std::string valOfDouble42AndAHalf(String::xtos(42.5));	// "42.500000"
		@endcode
	*/
	template <typename T>
	inline std::string xtofs(T arg)
	{
		static_assert(std::is_floating_point<T>::value, "Type should be floating point.");

		std::ostringstream buffer;
		buffer << std::fixed;
		buffer << arg;

		return buffer.str();
	}


	/**
		Splits a string delimited by a given character and stores the substrings in the given target vector.
		'elems' is cleared before receiving the resulting substrings.
		@param s the string to split
		@param delim the delimiter
		@param elems the vector to store the resulting substrings
		@return elems

		<b>Example</b>
		@code
		std::vector<std::string> res;
		String::split("my;delimited;string", ';', res);		// res is { "my", "delimited", "string"}
		@endcode
	*/
	std::vector<std::string> const & split(std::string const& s, char delim, std::vector<std::string>& elems);


	/**
		Splits a string delimited by a given character and returns the result as a vector of substrings.
		@param s the string to split
		@param delim the delimiter
		@return a vector of strings that stores the substrings

		<b>Example</b>
		@code
		const auto res(String::split("my;delimited;string", ';', res));		// res is { "my", "delimited", "string"}
		@endcode
	*/
	std::vector<std::string> split(std::string const& s, char delim);


	/**
		Trims the leading blank characters of a string.
		@param s the string to trim
		@return s
		@see <a href="https://www.cplusplus.com/reference/cctype/isspace/">std::isspace</a>

		<b>Example</b>
		@code
		std::string s("\t test\t ");
		String::ltrim(s);	// s is "test\t "
		@endcode
	*/
	std::string& ltrim(std::string& s); 


	/**
		Trims the trailing blank characters of a string.
		@param s the string to trim
		@return s
		@see <a href="https://www.cplusplus.com/reference/cctype/isspace/">std::isspace</a>

		<b>Example</b>
		@code
		std::string s("\t test\t ");
		String::rtrim(s);	// s is "\t test"
		@endcode
	*/
	std::string& rtrim(std::string& s); // 

	
	/**
		Trims the leading and trailing blank characters of a string.
		@param s the string to trim
		@return s
		@see <a href="https://www.cplusplus.com/reference/cctype/isspace/">std::isspace</a>

		<b>Example</b>
		@code
		std::string s("\t test\t ");
		String::trim(s);	// s is "test"
		@endcode
	*/
	std::string& trim(std::string& s); 


	/**
		Replaces all occurences of a substring in a string.
		@param source the string that contains substrings to replace
		@param find the substring to be replaced
		@param replaceBy the replacement substring
		@return source

		<b>Example</b>
		@code
		std::string s("Blue is blue");
		String::replace(s, "blue", "red");	// s is "Blue is red"
		@endcode
	*/
	std::string& replace(std::string& source, std::string const& find, std::string const& replaceBy);


	/**
		Converts a string to lowercase.
		@param s the string to convert
		@return source

		<b>Example</b>
		@code
		std::string s("BoTh CaSeS !");
		String::lower(s);	// s is "both cases !"
		@endcode
	*/
	std::string& lower(std::string& s);


	/**
		Returns a lowercase version of the given string.
		@param s the string to convert
		@return the lowercase version of source

		<b>Example</b>
		@code
		const std::string s("BoTh CaSeS !");
		const auto res(String::lower(s));	// res is "both cases !"
		@endcode
	*/
	std::string lower(std::string const& s);


	/**
		Converts a string to uppercase.
		@param s the string to convert
		@return source

		<b>Example</b>
		@code
		std::string s("BoTh CaSeS !");
		String::upper(s);	// s is "BOTH CASES !"
		@endcode
	*/
	std::string& upper(std::string& s);


	/**
		Returns a uppercase version of the given string.
		@param s the string to convert
		@return the uppercase version of source

		<b>Example</b>
		@code
		const std::string s("BoTh CaSeS !");
		const auto res(String::upper(s));	// res is "BOTH CASES !"
		@endcode
	*/
	std::string upper(std::string const& s);


	/**
		Returns an UTF-8 wstring version of the given string.
		@param str the string to convert
		@return the UTF-8 wstring version of str
		@see <a href="https://www.cplusplus.com/reference/codecvt/codecvt_utf8/">std::codecvt_utf8<wchar_t></a>

		<b>Example</b>
		@code
		const auto res(String::toWString("This is a test string."));	// res is L"This is a test string." 
		@endcode
	*/
	std::wstring toWString(std::string const& str);
	

	/**
		Returns a string version of the given UTF-8 wstring.
		@param wstr the UTF-8 wstring to convert
		@return the string version of wstr
		@see <a href="https://www.cplusplus.com/reference/codecvt/codecvt_utf8/">std::codecvt_utf8<wchar_t></a>

		<b>Example</b>
		@code
		const auto res(String::fromWString(L"This is a test string."));	// res is "This is a test string." 
		@endcode
	*/
	std::string fromWString(std::wstring const& wstr);


	/**
		Computes the <a href="https://en.wikipedia.org/wiki/Levenshtein_distance">Levenshtein distance</a> between two strings.
		@param s a string 
		@param t another string
		@return the Levenshtein distance between the two 

		<b>Example</b>
		@code
		const auto res(String::levenshteinDistance("a", "abc"));	// res is 2
		@endcode
	*/
	size_t levenshteinDistance(std::string const & s, std::string const & t);
}

}
