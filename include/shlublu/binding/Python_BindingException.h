#pragma once

/** @file
	Subpart of the Python module.

	See Python namespace documentation for details.
*/

#include <shlublu/util/Exceptions.h>


namespace shlublu
{

namespace Python
{

/**
	Python throws this exception should an issue happen.
*/
class BindingException : public ShlubluException
{
public:
	/**
		Constructor.
		@param message description of the issue
	*/
	explicit BindingException(const std::string& message)
		: ShlubluException(("Python binding: " + message).c_str())
	{}

	/**
		Constructor.
		@param message description of the issue
	*/
	explicit BindingException(const char* message)
		: BindingException(std::string(message))
	{}
};

}

}

