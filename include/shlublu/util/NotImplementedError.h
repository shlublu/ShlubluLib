#pragma once

/** @file 
	Exception thrown in case of missing implementation.

	See NotImplementedError class documentation for details.
*/

#include <stdexcept>
#include <string>

namespace shlublu
{

	/**
		Components of this library that have no implementation for the running platform use this exception at run time.
	*/
	class NotImplementedError : public std::logic_error
	{
	public:
		/**
			Constructor.
			@param message description of the issue
		*/
		explicit NotImplementedError(std::string const& message)
			: logic_error(message.c_str())
		{}

		/**
			Constructor.
			@param message description of the issue
		*/
		explicit NotImplementedError(char const* message)
			: logic_error(message)
		{}
	};

}
