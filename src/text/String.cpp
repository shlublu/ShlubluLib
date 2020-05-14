#include <shlublu/text/String.h>

#include <codecvt>
#include <cstring>
#include <functional>

#include <shlublu/util/Exceptions.h>

namespace shlublu
{

std::vector<std::string> const & String::split(std::string const& s, char delim, std::vector<std::string>& elems)
{
	std::stringstream ss(s);
	std::string item;

	elems.clear();

	while (std::getline(ss, item, delim))
	{
		elems.push_back(item);
	}

	elems.shrink_to_fit();

	return elems;
}


std::vector<std::string> String::split(std::string const& s, char delim)
{
	std::vector<std::string> elems;

	split(s, delim, elems);

	return elems;
}


// trim from start (in place)
std::string& String::ltrim(std::string& s)
{
	s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](int c) {return !std::isspace(c); }));
	return s;
}


// trim from end (in place)
std::string& String::rtrim(std::string& s)
{
	s.erase(std::find_if(s.rbegin(), s.rend(), [](int c) {return !std::isspace(c); }).base(), s.end());
	return s;
}


// trim from both ends (in place)
std::string& String::trim(std::string& s)
{
	return rtrim(ltrim(s));
}


std::string& String::replace(std::string& source, std::string const& find, std::string const& replace)
{
	if (find.length() < 1)
	{
		throw ShlubluException("String::replace(): seed to replace should not be empty.");
	}

	for (std::string::size_type i = 0; (i = source.find(find, i)) != std::string::npos;)
	{
		source.replace(i, find.length(), replace);
		i += replace.length();
	}

	return source;
}


std::string& String::lower(std::string& s)
{
	std::transform(s.begin(), s.end(), s.begin(), [](auto c) { return std::tolower(c); });

	return s;
}


std::string String::lower(std::string const& src)
{
	auto scopy(src);

	return lower(scopy);
}


std::string& String::upper(std::string& s)
{
	std::transform(s.begin(), s.end(), s.begin(), [](auto c) { return std::toupper(c); });

	return s;
}


std::string String::upper(std::string const& src)
{
	auto scopy(src);

	return upper(scopy);
}


std::wstring String::toWString(std::string const & str)
{
	std::wstring_convert<std::codecvt_utf8<wchar_t>> conv;
	return conv.from_bytes(str);
}


std::string String::fromWString(std::wstring const& wstr)
{
	std::wstring_convert<std::codecvt_utf8<wchar_t>> conv;
	return conv.to_bytes(wstr);
}


size_t String::levenshteinDistance(std::string const & s1, std::string const & s2)
{
	const size_t m(s1.size());
	const size_t n(s2.size());

	if (m == 0) return n;
	if (n == 0) return m;

	size_t* const costs(new size_t[n + 1]);

	for (size_t k = 0; k <= n; k++) costs[k] = k;

	size_t i(0);
	for (auto it1 = s1.begin(); it1 != s1.end(); ++it1, ++i)
	{
		costs[0] = i + 1;
		size_t corner = i;

		size_t j(0);
		for (auto it2 = s2.begin(); it2 != s2.end(); ++it2, ++j)
		{
			const size_t upper(costs[j + 1]);

			if (*it1 == *it2)
			{
				costs[j + 1] = corner;
			}
			else
			{
				const size_t t(upper < corner ? upper : corner);
				costs[j + 1] = (costs[j] < t ? costs[j] : t) + 1;
			}

			corner = upper;
		}
	}

	const size_t result(costs[n]);
	delete[] costs;

	return result;
}

}
