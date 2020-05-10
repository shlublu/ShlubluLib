#pragma once

#include <exception>
#include <string>


class ShlubluException : public std::exception
{
public:
#ifdef _WIN32
	explicit ShlubluException(const std::string & message)
		: exception(message.c_str())
	{}

	explicit ShlubluException(const char *message)
		: exception(message)
	{}
#else
	explicit ShlubluException(const std::string & message)
		: exception(),
		  mWhat(message)
	{}

	explicit ShlubluException(const char *message)
		: exception(),
		  mWhat(message)
	{}

	virtual const char* what() const noexcept 
	{ 
		return mWhat.c_str();
	}

private:
	const std::string mWhat;
#endif
};


class NotImplementedException : public ShlubluException
{
public:
	explicit NotImplementedException(const std::string& message)
		: ShlubluException(message.c_str())
	{}

	explicit NotImplementedException(const char* message)
		: ShlubluException(message)
	{}
};

