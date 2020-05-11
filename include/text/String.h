#pragma once

#include <algorithm>
#include <iomanip>
#include <sstream>
#include <vector>


namespace String
{
	// To String function
	template <typename T>
	inline std::string xtos(T arg)
	{
		static_assert(std::is_arithmetic<T>::value || std::is_pointer<T>::value, "Type should be either arithmetic or pointer.");

		std::ostringstream buffer;
		buffer << arg;

		return buffer.str();
	}

	template <typename T>
	inline std::string xtofs(T arg)
	{
		static_assert(std::is_floating_point<T>::value, "Type should be floating point.");

		std::ostringstream buffer;
		buffer << std::fixed;
		buffer << arg;

		return buffer.str();
	}

	// String manipulation functions
	std::vector<std::string> const & split(const std::string & s, char delim, std::vector<std::string> & elems);

	std::vector<std::string> split(const std::string & s, char delim);

	std::string& ltrim(std::string & s); // trim from start

	std::string& rtrim(std::string & s); // trim from end 

	std::string& trim(std::string & s); // trim from both ends 

	std::string& replace(std::string & source, std::string const & find, std::string const & replace);

	std::string& lower(std::string & s);

	std::string lower(std::string const& s);

	std::string& upper(std::string & s);

	std::string upper(std::string const& s);

	// Wide characters
	std::wstring toWString(std::string const& str);
	
	std::string fromWString(std::wstring const& wstr);

	// Diff
	size_t levenshteinDistance(std::string const & s, std::string const & t);
}

