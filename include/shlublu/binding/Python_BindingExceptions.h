#pragma once

/** @file
	Subpart of the Python module.

	See Python namespace documentation for details.
*/

#include <stdexcept>


namespace shlublu
{

namespace Python
{

/**
	Python throws this exception when an issue not detectable in the code happens at run time.
*/
class BindingRuntimeError : public std::runtime_error
{
public:
	/**
		Constructor.
		@param message description of the issue
	*/
	explicit BindingRuntimeError(const std::string& message)
		: std::runtime_error(("Python binding: " + message).c_str())
	{}

	/**
		Constructor.
		@param message description of the issue
	*/
	explicit BindingRuntimeError(const char* message)
		: std::runtime_error(std::string(message))
	{}
};


/**
	Python throws this exception when an issue expected to be detectable in the code happens at run time.
*/
class BindingLogicError : public std::logic_error
{
public:
	/**
		Constructor.
		@param message description of the issue
	*/
	explicit BindingLogicError(const std::string& message)
		: logic_error(("Python binding: " + message).c_str())
	{}

	/**
		Constructor.
		@param message description of the issue
	*/
	explicit BindingLogicError(const char* message)
		: logic_error(std::string(message))
	{}
};

}

}

