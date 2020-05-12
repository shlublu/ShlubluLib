#pragma once

/** @file 
	Named exceptions derived from <a href="https://www.cplusplus.com/reference/exception/exception/">std::exception</a>.
*/

#include <exception>
#include <string>


/** 
	All classes of ShlubluLib use this exception or a derived one when needed.
*/
class ShlubluException : public std::exception
{
public:
#ifdef _WIN32
	explicit ShlubluException(std::string const& message)
		: exception(message.c_str())
	{}

	explicit ShlubluException(char const *message)
		: exception(message)
	{}
#else
	/**
		Constructor.
		@param message description of the issue
	*/
	explicit ShlubluException(std::string const& message)
		: exception(),
		  mWhat(message)
	{}

	/**
		Constructor.
		@param message description of the issue
	*/
	explicit ShlubluException(char const* message)
		: exception(),
		  mWhat(message)
	{}

	/**
		Returns the description of the issue carried by this exception.
		@return the description of the issue as a C-string
	*/	
	virtual char const* what() const noexcept
	{ 
		return mWhat.c_str();
	}

private:
	const std::string mWhat;
#endif
};


/**
	Components of this library that have no implementation for the running platform use this exception at run time.
*/
class NotImplementedException : public ShlubluException
{
public:
	/**
		Constructor.
		@param message description of the issue
	*/
	explicit NotImplementedException(std::string const& message)
		: ShlubluException(message.c_str())
	{}

	/**
		Constructor.
		@param message description of the issue
	*/
	explicit NotImplementedException(char const* message)
		: ShlubluException(message)
	{}
};

