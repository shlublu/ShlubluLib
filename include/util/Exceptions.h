#pragma once

/** @file util/Exceptions.h
	Named exceptions derived from <a href="http://www.cplusplus.com/reference/exception/exception/">std::exception</a>.
*/

#include <exception>
#include <string>


/** 
	All components of this library use this exception when needed.
*/
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


/**
	Components of this library that are not implemented under a cetrain platform use this exception at run time.
*/
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

